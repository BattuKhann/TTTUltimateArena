import torch
import network
import uttt_engine
import os
import utils
from types import SimpleNamespace

@torch.compile(mode="reduce-overhead")
def compiled_policy_step(model, gpu_state):
    with torch.amp.autocast('cuda', dtype=torch.float16):
        logits = model(gpu_state)
        probs = torch.softmax(logits, dim=-1)
        return torch.multinomial(probs, num_samples=1).squeeze(-1).to(dtype=torch.int8)

def train():
    total_games = 200_000
    buffer_size = total_games // 2

    trajectory_size = 20_000_000
    epoch_size = 12288 
    total_steps = 10  
    num_inner_steps = 200

    state_buffers = [torch.zeros((buffer_size, 270), dtype=torch.int8, pin_memory=True) for _ in range(2)]
    action_buffers = [torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True) for _ in range(2)]

    gpu_states_int8 = [torch.zeros((buffer_size, 270), dtype=torch.int8, device='cuda') for _ in range(2)]
    gpu_states = [torch.zeros((buffer_size, 270), dtype=torch.float16, device='cuda') for _ in range(2)]
    gpu_actions = [torch.zeros(buffer_size, dtype=torch.int8, device='cuda') for _ in range(2)]

    transition_buffers_cpu = [
        SimpleNamespace(
            states=torch.zeros((epoch_size, 270), dtype=torch.int8, pin_memory=True),
            actions=torch.zeros((epoch_size), dtype=torch.int8, pin_memory=True),
            values=torch.zeros((epoch_size), dtype=torch.float16, pin_memory=True)
        )
        for _ in range(2)
    ]

    transition_buffers_gpu = [
        SimpleNamespace(
            states=torch.zeros((epoch_size, 270), dtype=torch.float16, device='cuda'),
            actions=torch.zeros((epoch_size), dtype=torch.float16, device='cuda'),
            values=torch.zeros((epoch_size), dtype=torch.float16, device='cuda')
        )
        for _ in range(2)
    ]

    model = network.PolicyNetwork().cuda()
        
    arena = uttt_engine.Arena(total_games,
                              state_buffers[0].numpy(), state_buffers[1].numpy(),
                              action_buffers[0].numpy(), action_buffers[1].numpy())

    copy_stream = torch.cuda.Stream()
    compute_stream = torch.cuda.Stream()

    h2d_ready = torch.cuda.Event()
    d2h_ready = torch.cuda.Event()
    inf_ready = torch.cuda.Event()
    compute_ready = torch.cuda.Event()

    arena.reset()

    # Init
    gpu_states_int8[0].copy_(state_buffers[0], non_blocking=True)
    gpu_states[0].copy_(gpu_states_int8[0], non_blocking=True)
    actions = compiled_policy_step(model, gpu_states[0])
    gpu_actions[0].copy_(actions, non_blocking=True)
    
    inf_ready.record()
    compute_ready.record()
    torch.cuda.synchronize()

    for epoch in range(total_steps):
        gpu_batch = 1
        cpu_batch = 0

        # Inference Loop
        for step in range(num_inner_steps):
            
            with torch.cuda.stream(copy_stream):
                copy_stream.wait_event(compute_ready)
                gpu_states_int8[gpu_batch].copy_(state_buffers[gpu_batch], non_blocking=True)
                gpu_states[gpu_batch].copy_(gpu_states_int8[gpu_batch], non_blocking=True)
                h2d_ready.record(copy_stream)
                copy_stream.wait_event(inf_ready)
                action_buffers[cpu_batch].copy_(gpu_actions[cpu_batch], non_blocking=True)
                
                d2h_ready.record(copy_stream)
            
            with torch.cuda.stream(compute_stream):
                compute_stream.wait_event(h2d_ready)
                actions = compiled_policy_step(model, gpu_states[gpu_batch])
                gpu_actions[gpu_batch].copy_(actions, non_blocking=True)
                
                inf_ready.record(compute_stream)
            
            d2h_ready.synchronize()
            arena.step(cpu_batch)

            compute_ready.record()
            gpu_batch, cpu_batch = cpu_batch, gpu_batch
                
        torch.cuda.synchronize()
        arena.flush_trajectory()

        #Training Loop
        total_samples = (trajectory_size // epoch_size) * 2
        for sample in range(total_samples):
            #sample and train
            pass

if __name__ == '__main__':
    train()
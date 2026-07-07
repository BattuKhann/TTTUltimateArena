import torch
import network
import uttt_engine
import os
import utils
import time

total_games = 1_000_000
buffer_size = total_games // 2
total_steps = 500
state_buffers = [torch.zeros((buffer_size, 270), dtype=torch.int8, pin_memory=True), torch.zeros((buffer_size, 270), dtype=torch.int8, pin_memory=True)]
action_buffers = [torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True), torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True)]
reward_buffers = [torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True), torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True)]
gpu_states = [torch.zeros((buffer_size, 270), dtype=torch.float16, device='cuda'), torch.zeros((buffer_size, 270), dtype=torch.float16, device='cuda')]
gpu_actions = [torch.zeros(buffer_size, dtype=torch.int8, device='cuda'), torch.zeros(buffer_size, dtype=torch.int8, device='cuda')]

model = network.PolicyNetwork().cuda()
arena = uttt_engine.Arena(total_games,
                          state_buffers[0].numpy(), state_buffers[1].numpy(),
                          action_buffers[0].numpy(), action_buffers[1].numpy(),
                          reward_buffers[0].numpy(), reward_buffers[1].numpy())

copy_stream = torch.cuda.Stream()
compute_stream = torch.cuda.Stream()

h2d_ready = torch.cuda.Event()
compute_ready = torch.cuda.Event()


with torch.cuda.stream(copy_stream):
    gpu_states[0].copy_(state_buffers[0], non_blocking=True)
    h2d_ready.record(copy_stream)

with torch.cuda.stream(compute_stream):
    compute_stream.wait_event(h2d_ready)
    with torch.amp.autocast('cuda', dtype=torch.float16):
        logits = model(gpu_states[0])
        actions = torch.multinomial(torch.softmax(logits, dim=-1), num_samples=1).squeeze(-1).to(dtype=torch.int8, non_blocking=True)
        gpu_actions[0].copy_(actions, non_blocking=True)
    compute_ready.record(compute_stream)
    
with torch.cuda.stream(copy_stream):
    copy_stream.wait_event(compute_ready)
    action_buffers[0].copy_(gpu_actions[0], non_blocking=True)

with torch.cuda.stream(copy_stream):
    gpu_states[1].copy_(state_buffers[1], non_blocking=True)
    h2d_ready.record(copy_stream)

torch.cuda.synchronize()

gpu_batch = 1
cpu_batch = 0

for step in range(total_steps):
    with torch.cuda.stream(compute_stream):
        compute_stream.wait_event(h2d_ready)
        with torch.amp.autocast('cuda', dtype=torch.float16):
            logits = model(gpu_states[gpu_batch])
            actions = torch.multinomial(torch.softmax(logits, dim=-1), num_samples=1).squeeze(-1).to(dtype=torch.int8, non_blocking=True)
            gpu_actions[gpu_batch].copy_(actions, non_blocking=True)
        compute_ready.record(compute_stream)
    
    with torch.cuda.stream(copy_stream):
        copy_stream.wait_event(compute_ready)
        action_buffers[gpu_batch].copy_(gpu_actions[gpu_batch], non_blocking=True)

    arena.step(cpu_batch)

    with torch.cuda.stream(copy_stream):
        gpu_states[cpu_batch].copy_(state_buffers[cpu_batch], non_blocking=True)
        h2d_ready.record(copy_stream)

    with torch.cuda.stream(copy_stream):
        copy_stream.synchronize()

    gpu_batch, cpu_batch = cpu_batch, gpu_batch

torch.cuda.synchronize()
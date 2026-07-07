import torch
import network
import uttt_engine
import os
import utils
import time

total_games = 100_000
buffer_size = total_games // 2
total_steps = 1_000
state_buffers = [torch.zeros((buffer_size, 270), dtype=torch.int8, pin_memory=True), torch.zeros((buffer_size, 270), dtype=torch.int8, pin_memory=True)]
action_buffers = [torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True), torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True)]
reward_buffers = [torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True), torch.zeros(buffer_size, dtype=torch.int8, pin_memory=True)]

model = network.PolicyNetwork().cuda()
arena = uttt_engine.Arena(total_games,
                          state_buffers[0].numpy(), state_buffers[1].numpy(),
                          action_buffers[0].numpy(), action_buffers[1].numpy(),
                          reward_buffers[0].numpy(), reward_buffers[1].numpy())

gpu_batch = 0
cpu_batch = 1

for step in range(total_steps):
    #H2D
    gpu_states = state_buffers[gpu_batch].cuda(non_blocking=True)
    gpu_states_float = gpu_states.to(dtype=torch.float32, non_blocking=True)

    #Inference + Sampling
    logits = model(gpu_states_float)
    actions = torch.multinomial(torch.softmax(logits, dim=-1), num_samples=1).squeeze(-1)
    actions.to(dtype=torch.int8, non_blocking=True)

    #D2H
    action_buffers[gpu_batch].copy_(actions, non_blocking=True)

    #Arena Step
    arena.step(cpu_batch)

    #Switch
    gpu_batch, cpu_batch = cpu_batch, gpu_batch

    torch.cuda.synchronize()
import subprocess

total_nodes_visited = 0
total_steps_taken = 0

executable_path = "./main" 

batch_size = 100

for _ in range(batch_size):
    print("run: " + str(_))
    result = subprocess.run([executable_path, "print"], capture_output=True, text=True)
    
    output_lines = result.stdout.strip().splitlines()
    
    if len(output_lines) != 2:
        raise ValueError("Executable output did not contain exactly 2 lines")

    try:
        nodes_visited = int(output_lines[0])
        steps_taken = int(output_lines[1])
    except ValueError:
        raise ValueError("Executable output lines are not valid integers")

    total_nodes_visited += nodes_visited
    total_steps_taken += steps_taken

print("Average nodes visited:", total_nodes_visited/batch_size)
print("Average steps taken:", total_steps_taken/batch_size)
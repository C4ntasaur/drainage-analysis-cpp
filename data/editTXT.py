# Define the function to remove leading and trailing spaces from a text file
def remove_spaces(input_file, output_file):
    with open(input_file, 'r') as infile:
        lines = infile.readlines()
    
    # Remove leading and trailing spaces for each line
    lines = [line.strip() for line in lines]
    
    # Write the modified content to a new file
    with open(output_file, 'w') as outfile:
        outfile.writelines(line + '\n' for line in lines)  # Ensure newlines are preserved

# Example usage
input_file = 'COP90.txt'  # Replace with the path to your input file
output_file = 'COP901.txt'  # Replace with the desired output file path

remove_spaces(input_file, output_file)

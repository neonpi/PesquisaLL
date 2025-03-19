line_jumps = dict()
line_jumps[25] = 2 + 25 + (25 + 1 + 2) + 1
line_jumps[50] = 2 + 50 + (50 + 1 + 3) + 1
line_jumps[100] = 2 + 100 + (100 + 1 + 5) + 1

print(line_jumps[25])
print(line_jumps[50])
print(line_jumps[100])

with open("i.txt","r") as i_files:

    for i_name in [i.strip() for i in i_files.readlines()]:
        tam = int(i_name.split("/")[4])
        with open(i_name, "r") as i_file:
            lines = [i.strip() for i in i_file.readlines()]
            printed = False
            for i in range(tam-1):
                line = lines[i + line_jumps[tam]].split(" ")
                line = [int(i) for i in line]
                if(sum(line) > 1 and not printed):
                    out_str =f"Instancia {i_name} tem duplicidade de locker pra mesmo no" 
                    with open('output.txt','a') as out:
                        out.write(out_str+'\n')
                        print(out_str)
                    printed = True
                # print(sum(line))
                # print(sum([int(k) for k in line]))

            
            


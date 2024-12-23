f_out=open('u','a')

f_out.write('\nteste')

f_out.close()

f_in = open('u','r')
print(f_in.read(5))
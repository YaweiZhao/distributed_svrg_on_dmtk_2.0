import re
import fileinput

def write2file(text, filename2, mod):
	f = open(filename2,mod)
	f.write(text)
	f.close()

num=127
value=0
counter=0
compute_time=0
wait_time=0
while num>=0:
	compute_time=0
	wait_time=0
        counter=0
	filename='loss'+str(num)+'.txt'
	f=open(filename)
	for line in f:
		loss=float(line)
		if(loss>=0.03):
			counter=counter+1
                else:
			break
	f.close()
	print counter
	counter_temp=counter
	filename='loss_time_footprint'+str(num)+'.txt'
	ff=open(filename,'r+')
	for line in ff:
		compute_time=compute_time+float(line)
		counter_temp=counter_temp-1
		if counter_temp<0:
			break
	write2file(str(compute_time)+'\n','total_compute_time.txt','a+')

	
	filename2='wait_time'+str(num)+'.txt'
	ff2=open(filename2,'r+')
	counter_temp=counter
	for line in ff2:
		wait_time=wait_time+float(line)
		counter_temp=counter_temp-1
		if counter_temp<0:
			break
	write2file(str(wait_time)+'\n','total_wait_time.txt','a+')
	num=num-1



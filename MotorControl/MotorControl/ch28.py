# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
import matplotlib.pyplot as plt 
from statistics import mean 
from genref import genRef


def read_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_int = list(map(int,dat_str.split())) # now the data is a list
        ref.append(dat_int[0])
        data.append(dat_int[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    print(n_int)
    plt.xticks(range(0, n_int + 25, int(n_int/4)), range(0, (int(n_int/5)+5), 5))
    plt.ylabel('Current (mA)')
    plt.xlabel('Time (ms)')
    plt.show()

def read_pos_plot_matrix():
    n_str = ser.read_until(b'\n');  # get the number of data points to receive
    n_int = int(n_str) # turn it into an int
    print('Data length = ' + str(n_int))
    ref = []
    data = []
    data_received = 0
    while data_received < n_int:
        dat_str = ser.read_until(b'\n');  # get the data as a string, ints seperated by spaces
        dat_int = list(map(int,dat_str.split())) # now the data is a list
        ref.append(dat_int[0])
        data.append(dat_int[1])
        data_received = data_received + 1
    meanzip = zip(ref,data)
    meanlist = []
    for i,j in meanzip:
        meanlist.append(abs(i-j))
    score = mean(meanlist)
    t = range(len(ref)) # index array
    plt.plot(t,ref,'r*-',t,data,'b*-')
    plt.title('Score = ' + str(score))
    plt.xticks(range(0, n_int + 200, 200), range(int(n_int/200)+1))
    plt.ylabel('Angle (deg)')
    plt.xlabel('Time (s)')
    plt.show()

ser = serial.Serial('COM5',230400)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\tc: get encoder counts \te: reset encoder \td: read encoder angle \tq: quit') # '\t' is a tab
    print('\tf: set pwm \tp: unpower motor \tr: get mode \t l: go to angle')
    print('\ta: get current \tg: set current gains \th: get current gains \tk: ITEST')
    print('\ti: set position gains \tj: get position gains \tm: generate step reference \tn: generate cubic reference \t o: track traj')
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'
     
    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array
    
    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'):
        print('Requesting current in milliamps:' ) # req current
        n_str = ser.read_until(b'\n');  # get current
        n_float = float(n_str)
        print('' + str(n_float) + ' milliamps\n') # print it to the screen

    elif (selection == 'c'):
        # read encoder counts
        print('Requesting encoder counts:' ) # req counts
        n_str = ser.read_until(b'\n');  # get counts
        n_int = int(n_str)
        print('Encoder counts = ' + str(n_int) + '\n') # print it to the screen
    
    elif (selection == 'd'):
        # read encoder angle
        print('Requesting encoder angle:' ) # req angle
        n_str = ser.read_until(b'\n');  # get angle
        n_float = float(n_str)
        print('Encoder angle: ' + str(n_float) + '\n') # print it to the screen

    elif (selection == 'e'): # reset encoder
        print('Requesting reset encoder \n')
    
    elif (selection == 'f'): # set pwm signal

        pwm = input('Set PWM (-100 to 100): ') # get input
        ser.write((pwm + '\n').encode()) # write input
        print("Setting PWM to: "+ pwm + "\n")

    elif (selection == 'g'): # set current gains
        p_gain = input('Set proportional gain: ')
        i_gain = input('Set integral gain: ')

        print("Setting gains to: "+ p_gain + " and " + i_gain + "\n")
        ser.write((p_gain + ' ' + i_gain + '\n').encode()) # send current gains


    elif (selection == 'h'): # request current gains
        print('Requesting current gains:')
        n_str = ser.read_until(b'\n');  # get gains
        print(str(n_str)[2:len(str(n_str))-5] + "\n") # print them out

    elif (selection == 'i'): # set position gains
        p_gain = input('Set proportional gain: ')
        i_gain = input('Set integral gain: ')
        d_gain = input('Set derivate gain: ')

        print("Setting gains to: "+ p_gain + ", " + i_gain + ", and " + d_gain + "\n")
        ser.write((p_gain + ' ' + i_gain + ' ' + d_gain + '\n').encode()) # write gains to uart

    elif (selection == 'j'): # get position gains
        print('Requesting position gains:')
        n_str = ser.read_until(b'\n');  # get gains
        print(str(n_str)[2:len(str(n_str))-5] + "\n") # print them out
    
    elif (selection == 'k'):
        read_plot_matrix() # read plot for current control

    elif (selection == 'l'): # set reference angle

        angle = input('Set reference angle: ') 
        print("Setting ref angle to: "+ angle + "\n")
        ser.write((angle + '\n').encode()) # send reference angle

    elif (selection == 'm'): # generate step and send
        ref = genRef('step')

        ser.write((str(len(ref)) + '\n').encode()) # send data # of pts

        for ref_value in ref:
            ser.write((str(ref_value) + '\n').encode()) # sending data as angle in degrees

        print(len(ref)) # display requested trajectory
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('value')
        plt.xlabel('index')
        plt.show()

    elif (selection == 'n'): # generate cubic wave
        ref = genRef('cubic')

        ser.write((str(len(ref)) + '\n').encode())

        for ref_value in ref:
            ser.write((str(ref_value) + '\n').encode()) # sending data as angle in degrees

        print(len(ref)) # display requested trajectory
        t = range(len(ref))
        plt.plot(t,ref,'r*-')
        plt.ylabel('value')
        plt.xlabel('index')
        plt.show()
    
    elif (selection == 'o'): # initialize trajectory tracking and read data bacl
        read_pos_plot_matrix()


    elif(selection == 'p'): # power motor off
        print("Setting Mode to IDLE\n")
        

    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        # be sure to close the port
        ser.close()

    elif (selection == 'r'):
        print('Requesting mode:')
        n_str = ser.read_until(b'\n');  # get mode
        n_int = int(n_str)

        if (n_int == 0): # print mode out
            print('Mode is IDLE\n')
        elif (n_int == 1):
            print('Mode is PWM\n')
        elif (n_int == 2):
            print('Mode is ITEST\n')
        elif (n_int == 3):
            print('Mode is HOLD\n')
        elif (n_int == 4):
            print('Mode is TRACK\n')
        else:
            print('Unrecognized mode')

    else:
        print('Invalid Selection ' + selection_endline)




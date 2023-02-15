#!/usr/bin/python
import csv
import argparse
import matplotlib.pyplot as plt
import numpy as np
import sys

# python3 plot.py -t "Latency, Application Time" -d output -f task_1_data_FIFO.csv task_1_data_RR.csv -l FIFO RR -x threads -y "time [s]" -r 1 -cx 0 -cy 2 -a 20 -p

# python3 plot.py -t "CPU Time" -d output -f task_1_data_FIFO.csv task_1_data_RR.csv -l FIFO RR -x threads -y "time [s]" -r 1 -cx 0 -cy 3 -a 20 -p
# python3 plot.py -t "Throughput, CPU Time/nthreads" -f results/throughput.csv -n 2 -l FIFO RR -x threads -y "time [s]" -r 2 -cx 0 0 -cy 2 1 -p
# python3 plot.py -t "Throughput, CPU Time/nthreads" -f results/throughputFull.csv -n 2 -l FIFO RR -x threads -y "time [s]" -r 2 -cx 0 0 -cy 2 1 -a 20 -p

# python3 plot.py -t "Waiting Time" -d output -f task_1_data_FIFO.csv task_1_data_RR.csv -l FIFO RR -x threads -y "time [s]" -r 1 -cx 0 -cy 4 -a 20 -p

def main():
    parser = argparse.ArgumentParser(description='plot header, files to plot (csv), labels for files')
    parser.add_argument('-t', '--title', type=str, required=True)
    parser.add_argument('-d', '--directory', type=str, help='path to directory location containing all files (not including ending \'/\')')
    parser.add_argument('-f', '--files', type=str, nargs='+', required=True, help="files to plot, same amount and order as labels")
    parser.add_argument('-n', '--numPlotsPerFile', type=int, default=1, help="number of plots that should be made per file (n*num_files == num_labels)")
    parser.add_argument('-l', '--labels', type=str, nargs='+', required=True, help="labels for plots, same amount and order as files (order: p1f1 p2f1 p1f2 p2f2 ...)")
    parser.add_argument('-x', '--xlabel', type=str, default='xlabel')
    parser.add_argument('-y', '--ylabel', type=str, default='ylabel')
    parser.add_argument('-r', '--row', type=int, default=0, help='row/line to start reading from in file/files') #ksk göra denna per file/per label???
    parser.add_argument('-cx', '--columnX', type=int, nargs='+', default=[0], help='column to read x value from in csv file/files') #ksk göra en per fil och plot (ist för en per plot i fil)???
    parser.add_argument('-cy', '--columnY', type=int, nargs='+', default=[1], help='column to read y value from in csv file/files')
    parser.add_argument('-a', '--avreage', type=int, default=1, help='avreage the data over n values')
    parser.add_argument('-p', '--print', action='store_true', help='print the data to console')

    args = parser.parse_args()
    print(args)

    if args.numPlotsPerFile == 1 and len(args.files) != len(args.labels):
        print('Amount of files needs to mach amount of labels when -n is 1')
        exit(1)
    if args.numPlotsPerFile * len(args.files) != len(args.labels):
        print('Amount of labels needs to match numPlotsPerFile * amount of files')
        exit(1)
    if args.numPlotsPerFile != len(args.columnX) or args.numPlotsPerFile != len(args.columnY):
        print('numPlotsPerFile need to match len of columnX and len of columnY')
        exit(1)

    path = args.directory+'/' if args.directory else ''

    plt.figure()
    plt.title(args.title)
    for i in range(len(args.files)):
        for j in range(args.numPlotsPerFile):
            if (args.print):
                print(args.labels[i+j]+':')
            data = processFileData(path + args.files[i], args.row, args.columnX[j], args.columnY[j], args.avreage, args.print)
            plt.plot(data[0], data[1], label=args.labels[i+j])
            #plt.errorbar(data[0], data[1], yerr=data[2], label=args.labels[i+j])
    plt.legend()
    plt.xlabel(args.xlabel)
    plt.ylabel(args.ylabel)
    plt.show()

def processFileData(filepath, row, x, y, avg=1, printData=False):
    file = open(filepath, 'r')
    for _ in range(row):
        file.readline()
    lines = csv.reader(file)
    dataX = []
    dataY = []
    #errY = []
    counter = 0
    tmpX = 0
    tmpY = 0
    # minY = sys.float_info.max
    # maxY = sys.float_info.min
    for line in lines:
        tmpX += float(line[x])
        valY = float(line[y])
        tmpY += valY
        # if (valY < minY):
        #     minY = valY
        # if (valY > maxY):
        #     maxY = valY
        
        counter += 1
        if (counter == avg):
            counter = 0
            tmpX /= avg
            tmpY /= avg
            if (printData):
                print(tmpX, tmpY)
            dataX.append(tmpX)
            dataY.append(tmpY)
            # errY.append(np.sqrt((maxY-minY)/2))
            tmpX = 0
            tmpY = 0
            # minY = sys.float_info.max
            # maxY = sys.float_info.min
    file.close()
    return (dataX, dataY) #, errY)

if __name__ == '__main__':
	main()
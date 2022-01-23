import os;
import re;
import numpy as np;

t_value = {
    "10": [],
    "100": [],
    "1000": [],
    "10000": [],
}

for t in t_value:
    for problem_size in range(2, 7):
        for repeat in range(1, 40):
            cmndName = "rose --iterations 1000 --problem_size " + str(problem_size) + " --temperature_function " \
                                                                                      "temperature_1000" + \
                       " --temperature_1000 " + t
            print(cmndName)
            result = os.popen(cmndName)
            output = result.read()
            calcTime = re.findall("dt.*", output)
            if (len(calcTime) > 0):
                calcTime = re.findall("[0-9.]+", calcTime[0])
                result_val = re.findall("[0-9.]+", re.findall("result.*", output)[0])

                t_value[t].append([problem_size, float(result_val[0]), float(calcTime[0])])

with open("result.plt", "a") as gnuplotfile:
    gnuplotfile.write("set terminal png\n")
    gnuplotfile.write("set output \"result.png\"\n")
    gnuplotfile.write("plot ")
    for t in t_value:
        print(t)
        summary = t_value[t]
        # print(summary)
        per_size = {}
        for values in summary:
            if (per_size.get(values[0]) is None):
                per_size[values[0]] = [[values[1], values[2]]]
            else:
                per_size[values[0]].append([values[1], values[2]])
        # print(per_size)
        for s in per_size:
            combined = np.mean(per_size[s], axis=0)
            with open("result_" + t + ".txt", "a") as myfile:
                myfile.write(str(s) + " " + str(combined[0]) + " " + str(combined[1]) + "\n")
        gnuplotfile.write("'result_" + t + ".txt' u 1:2 w lines, ")

    gnuplotfile.write("\n")

result = os.popen("gnuplot result.plt")
output = result.read()

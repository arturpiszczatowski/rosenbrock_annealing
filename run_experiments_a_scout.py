import os;
import re;
import numpy as np;

a_value = {
    "0.1": [],
    "0.2": [],
    "0.3": [],
    "0.4": [],
    "0.5": [],
    "0.6": [],
    "0.7": [],
    "0.8": [],
    "0.9": [],

}

for a in a_value:
    for problem_size in range(2, 7):
        for repeat in range(1, 40):
            cmndName = "rose --iterations 1000 --problem_size " + str(problem_size) + " --temperature_function " \
                                                                                      "temperature_a" + \
                       " --temperature_a " + a
            print(cmndName)
            result = os.popen(cmndName)
            output = result.read()
            calcTime = re.findall("dt.*", output)
            if (len(calcTime) > 0):
                calcTime = re.findall("[0-9.]+", calcTime[0])
                result_val = re.findall("[0-9.]+", re.findall("result.*", output)[0])

                a_value[a].append([problem_size, float(result_val[0]), float(calcTime[0])])

with open("result.plt", "a") as gnuplotfile:
    gnuplotfile.write("set terminal png\n")
    gnuplotfile.write("set output \"result.png\"\n")
    gnuplotfile.write("plot ")
    for a in a_value:
        print(a)
        summary = a_value[a]
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
            with open("result_" + a + ".txt", "a") as myfile:
                myfile.write(str(s) + " " + str(combined[0]) + " " + str(combined[1]) + "\n")
        gnuplotfile.write("'result_" + a + ".txt' u 1:2 w lines, ")

    gnuplotfile.write("\n")

result = os.popen("gnuplot result.plt")
output = result.read()

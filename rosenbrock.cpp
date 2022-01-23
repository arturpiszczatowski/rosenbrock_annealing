#include<iostream>
#include<functional>
#include<fstream>
#include<sstream>
#include<vector>
#include<random>
#include<algorithm>
#include<numeric>
#include<chrono>
#include<set>
#include<list>
#include<map>


using namespace std;

std::mt19937 rand_gen(time(nullptr));

void show_numbers(vector<double> numbers){
    cout << "[ ";
    for(int n : numbers) {
        cout << n << " ";
    }
    cout << "]";
}

vector<double> acquier_numbers(string file_name){
    vector<double> given_numbers;
    ifstream txt_file;
    txt_file.open(file_name);
    int number;

    if(!txt_file){
        cout << "File not found.";
    }

    while(txt_file >> number){
        given_numbers.push_back(number);
    }

    txt_file.close();

    return given_numbers;
}

double goal_function(vector<double> x){

    double result = 0;
    int iterations = x.size()-1;

    for(int i=0; i<iterations; i++) {
        result += 100 * pow(x[i + 1] - pow(x[i], 2), 2) + pow(1 - x[i], 2);
    }

    return result;
}

vector<double> generate_random_neighbour(vector<double> current_numbers){
    uniform_int_distribution<int> random_index(0, current_numbers.size()-1);
    uniform_int_distribution<int> random_mod(0,1);
    int n = 0;
    if(random_mod(rand_gen)){
        n = -1;
    } else {
        n = 1;
    }
    current_numbers[random_index(rand_gen)] += n;
    vector<double> neighbour_numbers = current_numbers;
    return neighbour_numbers;
}

double temperature_1000(int k, double s) {
    return s/k;
}

double temperature_log(int k, double l) {
    return 1/log10(k);
}

double temperature_a(int k, double a){
    if(a<0.0 || a>1.0) throw invalid_argument("Parameter a must be in domain (0,1).");
    return pow(a, k);
}

vector<double> generate_random_problem(int size){
    vector<double> problem;
    uniform_int_distribution<int> number(-10,10);
    for(int i=0; i<size; i++){
        problem.push_back(number(rand_gen));
    }

    return problem;
}


vector<double> simulated_annealing(function<double(vector<double>)> goal,
                                   vector<double> p0,
                                   int iterations,
                                   function<vector<double>(vector<double>)> neighbour,
                                   function<double(int, double)> T,
                                   double t_arg,
                                   function<void(int c, double dt)> on_statistics = [](int c, double dt){},
                                   function<void(int i, double current_goal_val, double goal_val)>
                                   on_iteration = [](int i, double current_goal_val, double goal_val) {}){

    auto start = chrono::steady_clock::now();

    vector<double> current_numbers = p0;
    vector<double> best_numbers = p0;
    int stat_goal_function_calls = 0;

    uniform_real_distribution<> u_k(0.0, 1.0);

    for(int i=0; i <iterations; i++){
        vector<double> neighbour_numbers = neighbour(current_numbers);
        if(goal(neighbour_numbers) < goal(current_numbers)){
            current_numbers = neighbour_numbers;
        } else {
            double u = u_k(rand_gen);
            if(u < exp(-abs(goal(neighbour_numbers) - goal(current_numbers)) / T(i, t_arg))){
                current_numbers = neighbour_numbers;
            } else {

            }
        }
        if(goal(current_numbers) < goal(best_numbers)){
            best_numbers = current_numbers;
        }
        stat_goal_function_calls++;
//        cout << i+1 << ") ";
//        show_numbers(current_numbers);
//        cout << " score: " << goal(current_numbers) << endl;

        on_iteration(stat_goal_function_calls, goal(current_numbers), goal(best_numbers));
    }

    auto finish = chrono::steady_clock::now();
    chrono::duration<double> duration = finish - start;
    on_statistics(stat_goal_function_calls, duration.count());


    return best_numbers;
}


map<string, string> args_to_map(vector<string> arguments){
    map<string, string> ret;
    string argname ="";
    for(auto param : arguments){
        if((param.size() > 2) && (param.substr(0, 2) == "--")){
            argname = param.substr(2);
        } else {
            ret[argname] = param;
        }
    }
    return ret;
}


int main(int argc, char** argv) {

    map<string, string> parameters = {
            {"problem_size", "5"},
            {"iterations", "500"},
            {"temperature_function", "temperature_1000"},
            {"temperature_1000", "1000"},
            {"temperature_a", "0.6"},
            {"print_result", "false"}
    };
    for(auto [k, v] : args_to_map(vector<string>(argv, argv + argc))){
        parameters[k] = v;
    }

    vector<double> numbers = generate_random_problem(stoi(parameters["problem_size"]));
    int iterations = stoi(parameters["iterations"]);
    string temperature_function_method = parameters["temperature_function"];


    auto on_finish =
            [](int c, double dt) {
                cout << "# count: " << c << "; dt:  " << dt << endl;
            };

    auto on_step = [&](int i, double current_goal_val, double goal_v) {
        cout << i << " " << current_goal_val << " " << goal_v << endl;
    };

    vector<double> best_solution;

    if(temperature_function_method == "temperature_1000") {

        best_solution = simulated_annealing(goal_function,
                                                           numbers,
                                                           iterations,
                                                           generate_random_neighbour,
                                                           temperature_1000,
                                                           stoi(parameters["temperature_1000"]),
                                                           on_finish,
                                                           on_step);

    } else if (temperature_function_method == "temperature_a") {

        best_solution = simulated_annealing(goal_function,
                                                           numbers,
                                                           iterations,
                                                           generate_random_neighbour,
                                                           temperature_a,
                                                           stod(parameters["temperature_a"]),
                                                           on_finish,
                                                           on_step);
    } else if (temperature_function_method == "temperature_log") {

        best_solution = simulated_annealing(goal_function,
                                                           numbers,
                                                           iterations,
                                                           generate_random_neighbour,
                                                           temperature_log,
                                                           0,
                                                           on_finish,
                                                           on_step);
    }

    if (parameters["print_result"] == "true") {
        cout << "# best " << parameters["temperature_function"] << " solution:\n";
        show_numbers(numbers);
        cout << " => ";
        show_numbers(best_solution);
        cout << endl;
    }
    cout << "# best " << parameters["temperature_function"] << " result: " <<
         goal_function(best_solution) << endl;

    return 0;
}

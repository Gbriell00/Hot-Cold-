#include <iostream>
#include <string>
#include <optional>
#include <stdexcept>
#include <random>
#include <cmath>


using ushort = unsigned short;

struct running_options{
    //tamanho da entrada N
    size_t length {0};
    //número secreto gerado
    size_t secret_number {0};

    //armazena valor da diferença
    size_t temp_diff_try{0};
    size_t actual_diff_try{0};
    bool delta_diff{0};

    bool first_try = true;
    bool found{0};
    bool stop{0};
};

//transforma o string que contém o tamanho de N em int
std::optional<int> string_to_int(const std::string& value){
    int temp {0};
    try{
        temp = std::stoi(value);

    }
    catch (const std::invalid_argument& e) {
    return {};
     } 

    catch (const std::out_of_range& e) {
    return {};
    }
    
  return temp;
}

//valida a entrada do número N do cli
running_options validate_number(int argc, char* argv[],running_options& options){
   
    for(int i = 1; i < argc; i++){
          std:: string argument{argv[i]};

          if(argument == "-N"){
            if(i+1 >= argc){
                std::cout<<"Missing value for N."<<std::endl;
                exit(1);
            }
              auto temp = string_to_int(argv[i+1]);

            if(!temp.has_value()){
               std::cout<<"The value provided for -N is invalid or out of range."<<std::endl;
                exit(1);
            }

            int val = temp.value();
            if(val <= 0){
                std::cout<<"The value provided for -N must be greater than 0."<<std::endl;
                exit(1);
            }
            
            options.length = static_cast<size_t>(val);
           
            ++i;
        } 

        else{
            std::cout<<"invalid input. Correct form \" -N number \"."<<std::endl;
        }
       
    }

  return options;
}
    
//gera o número secreto
running_options generate_number(const size_t N, running_options& options ){

//inicializando o motor Mersenne Twister com seed
std::random_device rd;
std::mt19937 gen(rd());

//definindo intervalo
std::uniform_int_distribution<> dis(1, N);

options.secret_number = dis(gen);
return options;
}

//calcula a distância absoluta
void distance_calc(int& guess, running_options& options){
    int diff = options.secret_number - guess;
    options.actual_diff_try = std::abs(diff);
}

//compara as distâncias entre a tentativa atual e a anterior
void compare_distances(running_options& options){

    if(options.actual_diff_try>options.temp_diff_try){
        options.delta_diff = true;
    }
    else{
        options.delta_diff = false;
    }


    options.temp_diff_try = options.actual_diff_try;
}

//verifica a tentativa
running_options validate_guess(int& guess, running_options& options){
    
    if(options.first_try){
       
                if(options.first_try and guess == options.secret_number){
                        std::cout<<"WOW, tha's a lucky one. Found the secret number in the first shot: "<< options.secret_number << std::endl;
                        options.found = true;
                        return options;
                    }
        distance_calc(guess, options);

         options.temp_diff_try = options.actual_diff_try;
         options.first_try = false;
         return options;
    }

    else{

        if(guess == options.secret_number){
            std::cout<<"congratulations! you found the secret number: "<< options.secret_number << std::endl;
            options.found = true;
            return options;
        }
        else{
        distance_calc(guess, options);
        compare_distances(options);
                if(options.delta_diff){

                    std::cout<<"getting colder: "<< guess << std::endl;
                    options.delta_diff = NULL;
                    return options;
                }
                else{
                    
                    std::cout<<"getting hotter: "<< guess <<std::endl;
                    options.delta_diff = NULL;
                    return options;
                }


        }
    }    
}

int main (int argc, char* argv[]){

std::cout<<"Welcome to hot_cold v1.0, by Gabriel Garcia"<<std::endl;
std::cout<<"-------------------------------------------"<<std::endl;
std::cout<<"   To start the game, enter \" -N number\".  "<<std::endl;
std::cout<<"-------------------------------------------"<<std::endl;
std::cout<<"The game will generate a secret number from"<<std::endl;
std::cout<<"[1,N], good luck!                         "<<std::endl;


running_options options;
options = validate_number(argc, argv, options);

generate_number(options.length, options);

while(!options.found or options.stop){

    int guess;

    std::cin>>guess;
    
    if(guess<0){
            std::cout<<"See you later, bye!"<< std::endl;
            options.stop = true;
            break;
        }


    if(guess>options.length){
        std::cout<<"Number out of range. Range between [1, "<< options.length <<"]" << std::endl;
        continue;
    }


    validate_guess(guess, options);

}

    return 0;
}
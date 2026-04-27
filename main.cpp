#include <iostream>
#include <string>
#include <optional>
#include <stdexcept>
#include <random>
#include <cmath>


using ushort = unsigned short;

//struct para armazenar informações do jogo
struct running_options{
    //tamanho da entrada N
    size_t length {0};
    //número secreto gerado
    size_t secret_number {0};

    //armazena valor da diferença
    size_t temp_diff_try{0};
    size_t actual_diff_try{0};
    
    //usado para comparar se a diferença é maior ou menor
    bool delta_diff{0};

    //programa funciona diferente na primeira tentativa 
    bool first_try = true;

    //variável de controle para o loop do jogo
    bool found = false;
    bool stop = false;

    //liga ou desliga a cpu do jogo
    bool autoplay = false;

    //0 = gerar secret number, 1 = gerar primeira tentativa da cpu
    bool flag_generate_number = 0;
};

//struct para armazenar informações do modo automático
struct cpu_autoplay{
    std::vector<int> list_possible_numbers {};
    size_t secret_number_id {0};
    size_t first_guess {0};
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
running_options validate_arguments(int argc, char* argv[],running_options& options){
   
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
        else if(argument == "-autoplay"){
            options.autoplay = true;
        }
        else{
            std::cout<<"invalid input. Correct form \" -N number or -N number -autoplay\"."<<std::endl;
        }
       
    }

  return options;
}
    
//gera o número secreto
void generate_number(const size_t N, running_options& options, cpu_autoplay& cpu){

//inicializando o motor Mersenne Twister com seed
std::random_device rd;
std::mt19937 gen(rd());

//definindo intervalo
std::uniform_int_distribution<> dis(1, N);

if(options.flag_generate_number == 0){
options.secret_number = dis(gen);
}

if(options.flag_generate_number == 1){
    cpu.first_guess = dis(gen);
}
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

cpu_autoplay initialize_vector (cpu_autoplay& cpu, running_options& options){
    for(size_t i = 0; i < options.length; i++){
        //adiciona o número ao final do vetor
        cpu.list_possible_numbers.push_back(i+1);
    }
    cpu.secret_number_id = options.secret_number - 1;
    return cpu;
}

cpu_autoplay first_guess(cpu_autoplay& cpu, running_options& options){
    options.flag_generate_number = 1;
    generate_number(options.length, options, cpu);
    std::cout<<"First guess: "<< cpu.first_guess << std::endl;
    return cpu;
}


int main (int argc, char* argv[]){

std::cout<<"Welcome to hot_cold v1.0, by Gabriel Garcia"<<std::endl;
std::cout<<"-------------------------------------------"<<std::endl;
std::cout<<"   To start the game, enter \" -N number\".  "<<std::endl;
std::cout<<"-------------------------------------------"<<std::endl;
std::cout<<"The game will generate a secret number from"<<std::endl;
std::cout<<"[1,N], good luck!                         "<<std::endl;


running_options options;
cpu_autoplay cpu;
int guess;


options = validate_arguments(argc, argv, options);

generate_number(options.length, options, cpu);

while(!options.found or options.stop){

    //modo manual
    if (options.autoplay == false){
   

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
    //modo automático
    else{
        
        if (cpu.list_possible_numbers.empty()) {
         initialize_vector(cpu, options);
        }   
       
        
        if(options.first_try){
        first_guess(cpu, options);
        guess = cpu.first_guess;

        validate_guess(guess, options);
        
        }

            for (size_t i = 0; i < cpu.list_possible_numbers.size(); i++) {
              std::cout << cpu.list_possible_numbers[i] << " ";
                }
                std::cout << std::endl;

        std::cout<<"Secret number: "<< options.secret_number << std::endl;
        std::cout<<"Secret number id: "<< cpu.secret_number_id << std::endl;

}
    return 0;
}
}
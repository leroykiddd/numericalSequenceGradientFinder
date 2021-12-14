/*
    IVT1-20
    Petrov Denis Alexeevich
*/
 
 
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <chrono> // используется для определения времени работы программы
#include <string>
#include <vector> // используется как удобная альтернатива классического способа задания массивов через указатели
 
using namespace chrono;
 
void nextValue(vector<float>& vec, float n)
{
    vec.push_back(n);
}
 
float getFirstValue(vector<float>& vec)
{
    return vec.front();
}
 
void createNewGradient(vector<float>& vecP, vector<float>& vecN) {
    vecP = vecN;
    vecN.clear();
}
 
int main()
{
    // инициализация массива для хранения последовательности
    float sequenceArray[100];   
    int sequenceArrayLength = 0;
 
    //  получение имени файла
    std::cout << "Введите путь к файлу: ";
    string path;
    std::cin >> path;
    std::cout << endl;
 
    // попытка открытия файла, в случае неудачи - вывод сообщения об ошибке и завершение работы функции main (приложения)
    std::ifstream file(path);
    if (!file) {
        std::cout << "Не получилось открыть файл! Возможно неправильно введен путь." << endl;
        return 1;
    }
 
    // чтение float значений из файла и вывод их пользователю (в случае неудачи - завершение работы)
    string tempStringElement = "";
    std::cout << endl << "Входные данные" << endl;
 
    while (!file.eof()) {
        file >> tempStringElement;
        if (sscanf(tempStringElement.c_str(), "%f", &sequenceArray[sequenceArrayLength++]) != 1) {
            std::cout << endl << "Некорректный элемент в файле.";
            return 2;
        }
        std::cout << sequenceArray[sequenceArrayLength - 1] << " ";
    }
 
    file.close();
 
    // Основная часть
    float tempNum = 0.0f;
    vector<float> temp;
    float gradientPower = 0;
 
    vector<float> result;
 
    auto begin = chrono::steady_clock::now(); // временная метка начала работы ассемблерной вставки
    __asm {
 
        mov ebx, 1                  
        mov ecx, 1                  
 
        FLD[sequenceArray]                  
 
        Cycle:
 
        FLD[sequenceArray + ecx * 4]            
            FCOMI st, st(1)             
            jb newSequence              
 
            FXCH st(1)                  
 
            cmp ebx, 1                  
            je FirstElement         
 
 
            FSTP tempNum                
            FST tempNum                 
            mov eax, tempNum            
 
            jmp addNextGradientValue    
 
            FirstElement :
 
        FSTP tempNum                
            mov eax, tempNum            
 
            push ecx                    
            push eax                    
            lea eax, temp               
            push eax                    
            call nextValue  
            add esp, 8                  
            pop ecx                     
 
            FST tempNum                 
            mov eax, tempNum            
 
            addNextGradientValue :
 
        push ecx                    
            push eax                    
            lea eax, temp               
            push eax                    
            call nextValue  
            add esp, 8                  
            pop ecx                     
 
            inc ebx                     
 
            jmp NextNum                 
 
            newSequence :
 
        cmp ebx, 1                  
            jbe OneElementGradientEx    
 
            push ecx                    
            lea eax, temp               
            push eax                    
            call getFirstValue      
            add esp, 4                  
            pop ecx                     
 
            FSUBP st(2), st             
            FXCH st(1)                  
            FABS                        
            FLD gradientPower           
            FCOMIP st, st(1)            
            jb SaveNewMaxGradient       
            FSTP tempNum                
            jmp NextNum                 
 
            SaveNewMaxGradient :
 
        lea eax, gradientPower      
            FSTP dword ptr[eax]         
 
            push ecx                    
 
            lea ebx, temp               
            push ebx                    
            lea eax, result
            push eax                    
            call createNewGradient          
 
            add esp, 8                  
            pop ecx                     
 
            mov ebx, 1                  
 
            jmp NextNum
 
            OneElementGradientEx :
 
        FXCH st(1)                  
            FSTP tempNum                
 
            NextNum :
 
        cmp ecx, sequenceArrayLength            
            je EndCycle                 
 
            inc ecx                     
            jmp Cycle                   
 
            EndCycle :
 
 
    }
 
    auto end = steady_clock::now(); // временная метка завершения работы ассемблерной ставки
    auto elapsed_mcs = duration_cast<microseconds>(end - begin); // время работы
    std::cout << endl;
 
    // вывод результатов работы (в консоль и в файл output.txt)
    std::ofstream resultFile;
    resultFile.open("C:\\output.txt");
    std::cout << endl << "Результат" << endl << endl;
    std::cout << "Время работы(mcs): " << to_string(elapsed_mcs.count()) << endl;
    std::cout << "Памяти использовано: " << sizeof(result) + sizeof(path) + sizeof(sequenceArray) + sizeof(sequenceArrayLength) + sizeof(gradientPower) << "байт" << endl << endl;
 
    std::cout << "Сила градиента: " << gradientPower << endl;
    std::cout << "Значения градиента: ";
 
    for (size_t i = 0; i < result.size(); ++i)
    {
        std::cout << result.at(i) << " ";
        resultFile << result.at(i) << " ";
    }
 
    std::cout << endl << endl;
    resultFile.close();
 
    system("pause");
}
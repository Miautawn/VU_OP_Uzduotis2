#include "utility_functions.hpp"

template <class Container>
void sort_container(Container &items) {
    std::sort(items.begin(), items.end());
} 


bool yes_or_no(string text)
{
  string option;
  while(option != "y" && option != "n")
  {
    cout<<text + " y/n: ";
    cin>>option;
  }
  return option == "y";
}

bool is_digit(string input)
{
  int n = input.length();
  for(int i = 0; i<n; i++) if(!std::isdigit(input[i])) return 0;
  return 1;
}

int input_integer(string text, string err_text, int lower_bound, int upper_bound, string exit_string)
{
  string input;
  cout<<text;
  while(true)
  {
    cin>>input;
    if (input == exit_string) return -1;
    if(is_digit(input)) {
      int number = std::stoi(input);
      if(number >= lower_bound && number <= upper_bound) return number;
    }
    //jeigu neatitiko sąlygų
    cout<<err_text;
  }
}

float calculate_mean(int n, vector<int> grades)
{
  return float(std::accumulate(grades.begin(), grades.end(), 0)) / n;
}

float calculate_median(int n, vector<int> grades)
{
  if(grades.size() != 0) {
    sort_container(grades);
    //jei nelyginis skaičius namų darbų
    if(n%2 != 0) return grades[n / 2];
    //jei lyginis skaičius namų darbų
    else return ((float)(grades[n / 2] + grades[n / 2 - 1]))/2;
  } else return 0;
}

bool files_exists(const string &name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

#include "student_functions.hpp"

void generate_grades(int &n, Student &new_student, bool log)
{
    RandomInt generator;
    //jeigu vartotojas nepasakė kiek turi būti pažymių
    if(n == -1) n = generator.rnd(0, 15); 

    new_student.grades.reserve(n);
    for(int i=0; i<n; i++) new_student.grades.push_back(generator.rnd(0,10));

    if(log) {
      cout<<"\nBuvo sugeneruoti tokie "<<n<<" ND pažymiai: ";
      for(int grade: new_student.grades) cout<<grade<<" ";
    }
    new_student.exam_score = generator.rnd(0,10);
    if(log) cout<<"\nIr egzamino rezultatas: "<<new_student.exam_score<<endl;
}

void calculate_final(int grade_num, Student &new_student, bool is_mean)
{
  new_student.is_mean = is_mean;
  //jeigu yra nd pažymių
  if(grade_num != 0) {

    //skaičiuoti per vidurkį
    if(is_mean) { 
      new_student.mean = calculate_mean(grade_num, new_student.grades);
      new_student.final_score_mean = 0.4 * new_student.mean + 0.6 * new_student.exam_score;
    }
    //skaičiuoti per medianą 
    if(!is_mean) {  
      new_student.median = calculate_median(grade_num, new_student.grades);
      new_student.final_score_median = 0.4 * new_student.median + 0.6 * new_student.exam_score;
    } 
  }
  //jeigu pažymių iš ND nebuvo 
  else {
    new_student.mean, new_student.median = 0, 0;
    new_student.final_score_mean = 0.6 * new_student.exam_score;
    new_student.final_score_median =  0.6 * new_student.exam_score;
  }
}

void student_benchmark_generate_file(int n, string file_name)
{
  vector<Student> generated_students;
  Student bench_student;
  std::ostringstream buffer;
  buffer<<"VARDAS          PAVARDĖ"<<endl;

  for(int i = 0; i<n; i++) {

    //duomenų generavimas
    bench_student.grades.clear();
    bench_student.name = "Vardas" + std::to_string(i+1);
    bench_student.last_name = "Pavarde" +  std::to_string(i+1);
    int grade_num = -1;
    generate_grades(grade_num, bench_student, false);

    //studento buferizavimas
    buffer<<left<<setw(15)
          <<bench_student.name<<" "<<setw(30)
          <<bench_student.last_name;

    for(int j = 0; j<bench_student.grades.size(); j++) 
    buffer<<left<<setw(5)<<bench_student.grades[j]<<setw(5);

    buffer<<bench_student.exam_score<<endl;
  }

  //išvedimas mūsų benchmark failo
  ofstream output(file_name);
  output << buffer.str();
  output.close();  
}


template <class Container>
void student_benchmark(Container bench_students, string container_code,  string split_mode)
{
  int stages[5] = {1000, 10000, 100000, 1000000, 10000000};
  double full_time;
  Timer m_timer;
  string local_file;
  
  for(int stage_index = 0; stage_index < 5; stage_index++) {

    cout<<"\n***************************";
    cout<<"\nPradedamas tikrinimas su "<<stages[stage_index]<<" studentų..."<<endl;
    m_timer.reset();
    full_time = 0;
    local_file = "bench_temp" + std::to_string(stages[stage_index]) + ".txt";

    //failo kūrimas
    if(!files_exists(local_file)) {
      student_benchmark_generate_file(stages[stage_index], local_file);
      cout<<stages[stage_index]<<" studentų generavimas ir failo kūrimas užtruko: "
      <<m_timer.split_time(full_time)<<endl;
    }
    
    //failo nuskaitymas
    read_students_from_file(bench_students, 
    "bench_temp" + std::to_string(stages[stage_index]) + ".txt", false);
    cout<<stages[stage_index]<<" studentų nuskaitymas užtruko: "
    <<m_timer.split_time(full_time)<<endl;
    
    //studentų rūšiavimas į dvi grupes:
    ///////////////////////////////////

    //1) surušiavimas didėjimo tvarka
    sort_container(bench_students);
    cout<<stages[stage_index]<<" studentų surūšiavimas didėjimo tvarka užtruko: "
    <<m_timer.split_time(full_time)<<endl;

    //2) pirmo >= 5 radimas
    typename Container::iterator first_good_student;
    if(container_code == "VECTOR") {
      first_good_student = std::find_if(bench_students.begin(), bench_students.end(), 
      [](const Student student) {
        return student.final_score_mean >= 5;});

    }else {
      first_good_student = std::lower_bound(bench_students.begin(), bench_students.end(), 5,
      [](const Student &l_student, const int value) {
      return l_student.final_score_mean < value;});
    }
    
    

    //3) kietaku ir varguoliu į skirtingus konteinerius paskirstymas
    Container temp_kietuoliai;
    Container temp_varguoliai;
    Container *kietuoliai = &temp_kietuoliai;
    Container *varguoliai = &temp_varguoliai;
    if(split_mode == "COPY") {
      if(container_code == "VECTOR") {
        int varguoliai_size = std::distance(bench_students.begin(), first_good_student);
        int kietuoliai_size = stages[stage_index] - varguoliai_size;
        temp_kietuoliai.resize(kietuoliai_size);
        temp_varguoliai.resize(varguoliai_size);
        std::copy(bench_students.begin(), first_good_student, temp_varguoliai.begin());
        std::copy(first_good_student, bench_students.end(), temp_kietuoliai.begin());
      }else {
        temp_kietuoliai = {first_good_student, bench_students.end()};
        temp_varguoliai = {bench_students.begin(), first_good_student};
      }
      
    } else {
      temp_kietuoliai = {first_good_student, bench_students.end()};
      bench_students.erase(first_good_student, bench_students.end());
      varguoliai = &bench_students;
    }

    cout<<stages[stage_index]<<" Įrašų 'kietakų' ir 'varguolių' rūšiavimas užtruko: "
    <<m_timer.split_time(full_time)<<endl;

    ///////////////////////////////////
    //studentų rūšiavimas į dvi grupes:
    

    //kietakų išvedimas į failą
    local_file = "Benchmark/bench_kietuoliai" + std::to_string(stages[stage_index]) + ".txt";
    output_students(*kietuoliai, true, local_file, false);
    cout<<stages[stage_index]<<" Įrašų 'kietuolių' išvedimas į failą užtruko: "
    <<m_timer.split_time(full_time)<<endl;

    //nabagėlių išvedimas į failą
    local_file = "Benchmark/bench_varguoliai" + std::to_string(stages[stage_index]) + ".txt";
    output_students(*varguoliai, true, local_file, false);
    cout<<stages[stage_index]<<" Įrašų 'varguolių' išvedimas į failą užtruko: "
    <<m_timer.split_time(full_time)<<endl;

    //galutinio laiko išrašymas
    cout<<stages[stage_index]<<" VISAS TESTAS UŽTRUKO: "<<full_time<<endl;

    if(stage_index != 4) {
      if(!yes_or_no("Ar norite testi?")) break;
      else bench_students.clear(); 
    }
  }
}

template void student_benchmark(vector<Student> bench_students, string container_code, string split_mode);
template void student_benchmark(list<Student> bench_students, string container_code, string split_mode);
template void student_benchmark(deque<Student> bench_students, string container_code, string split_mode);

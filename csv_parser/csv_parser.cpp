#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

bool read_csv(
  const std::string &filename, const uint32_t &tail_lines);

int main(int argc, char **argv)
{
  if(argc < 3){
    std::cerr << "csv_parser [file] [tail_lines]" << std::endl;
    return -1;
  }

  std::string filename {argv[1]};
  const uint32_t tail_lines {static_cast<uint32_t>(atoi(argv[2]))};

  read_csv(filename, tail_lines);
  return 0;
}

bool read_csv(
  const std::string &filename,
  const uint32_t &tail_lines)
{
  std::string tail_filename{ filename.substr(0,filename.length()-4) };  
  std::stringstream filename_append;
  filename_append << "_t" << tail_lines << ".csv";
  tail_filename.append(filename_append.str());

  uint32_t line_no{};

  {
    std::ifstream csv_file(filename.c_str());
    std::string line_in;

    if (!csv_file.good())
      return false;

    while (!csv_file.eof()) {
      getline(csv_file, line_in);
      line_no++;
    }

    csv_file.close();
  }

  const uint32_t line_count {line_no};

  if(line_count <= tail_lines){
    return false;
  }

  {
    std::ifstream csv_file(filename.c_str());
    std::ofstream tail_csv_file(tail_filename.c_str());
    std::string line_in;
    line_no = 0;
    const uint32_t start_line { line_count - tail_lines - 1U };

    if (!csv_file.good())
      return false;

    if(!tail_csv_file.good()) {
      csv_file.close();
      return false;
    }

    while (!csv_file.eof()) {
      getline(csv_file, line_in);
      line_no++;

      if(line_no == 1 || line_no >= start_line){
        tail_csv_file << line_in << '\n';
      }
    }

    csv_file.close();
    tail_csv_file.close();
  }

  return true;  
}


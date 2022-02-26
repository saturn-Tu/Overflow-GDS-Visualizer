#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using boost::is_any_of;

struct IntPoint3D {
  int x_, y_, z_;
};

vector< pair<IntPoint3D, int> > of_h_points, of_v_points;
int cell_width, cell_height;
int overflow_layer = 2000;
int wire_w = 4;

void outputGDSRectangle(std::ofstream& fs, int layer, int lx, int ly, int w, int h) {
  fs << "b{" << layer << " xy(" << lx << " " << ly << " " << lx+w << " " << ly << " " 
    << lx+w << " " << ly+h << " " << lx << " " << ly+h << ")}\n";
}

void outputDesignGDT(const char *filename) {
  ofstream fs(filename);
	if (!fs)	{
		std::cerr << "File " << filename << " not exists!" << endl;
		exit(1);
	}
  cout << "#start output GDT file: " << filename << endl;
  fs << "gds2{600\n";
  fs << "m=201809-14 14:26:15 a=201809-14 14:26:15\n";
  fs << "lib \'asap7sc7p5t_24_SL\' 0.00025 2.5e-10\n";
  fs << "cell{c=201809-14 14:26:15 m=201809-14 14:26:15 \'AND2x2_ASAP7_75t_SL\'\n";
  // output overflow
  for(auto& of:of_h_points) {
    auto& p = of.first;
    outputGDSRectangle(fs, p.z_+overflow_layer, (p.x_+0.5)*cell_width+wire_w, (p.y_+0.5)*cell_height-wire_w, cell_width-2*wire_w, 2*wire_w);
  }
  for(auto& of:of_v_points) {
    auto& p = of.first;
    outputGDSRectangle(fs, p.z_+overflow_layer, (p.x_+0.5)*cell_width-wire_w, (p.y_+0.5)*cell_height+wire_w, 2*wire_w, cell_height-2*wire_w);
  }
  fs << "}\n}";
  fs.close();
}

// ./program filename cell_width cell_height
int main(int argc, char** argv) {
  if(argc < 5) {
    cout << "./program i_filename cell_width cell_height o_filename\n";
    cout << "defalut w/h: 32 40\n";
    exit(1);
  }
  cell_width = stoi(argv[2]);
  cell_height = stoi(argv[3]);
  
  ifstream fin(argv[1]);
  string tmp;
  if (!fin) {
    std::cerr << "File " << argv[1] << " not exists!" << endl;
    exit(1);
  }
  int total_of, max_of, wirelength, via_num, total_wl;
  vector<string> tmp_strings;
  getline(fin, tmp);
  boost::algorithm::split(tmp_strings, tmp, is_any_of(" "));
  total_of = stoi(tmp_strings.at(3));
  getline(fin, tmp);
  boost::algorithm::split(tmp_strings, tmp, is_any_of(" "));
  max_of = stoi(tmp_strings.at(3));
  getline(fin, tmp);
  boost::algorithm::split(tmp_strings, tmp, is_any_of(" "));
  wirelength = stoi(tmp_strings.at(2));
  getline(fin, tmp);
  boost::algorithm::split(tmp_strings, tmp, is_any_of(" "));
  via_num = stoi(tmp_strings.at(3));
  getline(fin, tmp);
  boost::algorithm::split(tmp_strings, tmp, is_any_of(" "));
  via_num = stoi(tmp_strings.at(4));
  getline(fin, tmp);
  getline(fin, tmp);
  IntPoint3D p1, p2;
  int dem, cap;
  while (true) {
    getline(fin, tmp);
    boost::algorithm::split(tmp_strings, tmp, is_any_of("(),- "), boost::token_compress_on);
    int offset = 0;
    for(auto token:tmp_strings) {
      if(isdigit(token[0]))
        break;
      offset++;
    }
    if((int)tmp_strings.size()-offset < 8)
      break;
    p1.x_ = stoi(tmp_strings.at(0+offset));
    p1.y_ = stoi(tmp_strings.at(1+offset));
    p1.z_ = stoi(tmp_strings.at(2+offset));
    p2.x_ = stoi(tmp_strings.at(3+offset));
    p2.y_ = stoi(tmp_strings.at(4+offset));
    p2.z_ = stoi(tmp_strings.at(5+offset));
    dem = stoi(tmp_strings.at(6+offset));
    cap = stoi(tmp_strings.at(7+offset));
    if(dem > cap) {
      if(p1.x_ == p2.x_) {
        of_v_points.emplace_back(p1, dem-cap);
      } else {
        of_h_points.emplace_back(p1, dem-cap);
      }
    }
  }
  cout << "#Horizontal overflow\n\n";
  for(const auto& point : of_h_points) {
    cout << point.first.x_ << " " << point.first.y_ << " " <<  point.first.z_ << ": " << point.second << endl;
  }
  cout << "\n#Vertical overflow\n\n";
  for(const auto& point : of_v_points) {
    cout << point.first.x_ << " " << point.first.y_ << " " <<  point.first.z_ << ": " << point.second << endl;
  }
  outputDesignGDT(argv[4]);
  return 0;
}
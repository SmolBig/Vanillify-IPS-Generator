#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

const std::vector<uint8_t> IPS_HEADER = { 'P', 'A', 'T', 'C', 'H' };
const std::vector<uint8_t> IPS_FOOTER = { 'E', 'O', 'F' };

class Chunk {
public:
  Chunk(const std::string& str) {
    int sult = sscanf_s(str.data(), "%x: %x", &address, &value);
    if(sult != 2) { throw "DERP!"; }
  }

  bool operator<(const Chunk& other) const {
    return address < other.address;
  }

  std::vector<uint8_t> getData() const {
    std::vector<uint8_t> data;

    //convert address to data
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&address);
    ptr += 2;
    data.push_back(*ptr--);
    data.push_back(*ptr--);
    data.push_back(*ptr--);

    //generate length data
    uint8_t length = (value > 0xFF) ? 2 : 1;
    data.push_back(0);
    data.push_back(length);

    //convert value to data
    ptr = reinterpret_cast<const uint8_t*>(&value);
    while(length--) {
      data.push_back(*ptr++);
    }

    return data;
  }

private:
  int address;
  int value;

};

int main() {
  std::ifstream f("../master patch list.txt", std::ios::binary);

  std::vector<Chunk> chunks;

  std::string line;
  while(f.good()) {
    line.clear();
    std::getline(f, line);
    if(line.empty()) { break; }
    chunks.emplace_back(line);
  }

  f.close();

  std::sort(chunks.begin(), chunks.end(), [](const Chunk& a, const Chunk& b) { return a < b; });

  std::vector<char> outData(IPS_HEADER.begin(), IPS_HEADER.end());
  for(auto chunk : chunks) {
    auto cdata = chunk.getData();
    outData.insert(outData.end(), cdata.begin(), cdata.end());
  }
  outData.insert(outData.end(), IPS_FOOTER.begin(), IPS_FOOTER.end());

  std::ofstream patch("../vanillify.ips", std::ios::binary);
  patch.write(outData.data(), outData.size());

}


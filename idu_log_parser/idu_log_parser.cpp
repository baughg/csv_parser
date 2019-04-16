#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

bool read_csv(
    const std::string &filename, 
    std::map<uint32_t, uint32_t> &cum_trans,
    std::map<uint32_t, uint32_t> &latency);

int main()
{
    std::map<uint32_t, uint32_t> cum_trans;
    std::map<uint32_t, uint32_t> latency;
    read_csv("Reader_ACT(0).csv", cum_trans, latency);
    read_csv("Reader_ACT(1).csv", cum_trans, latency);
    read_csv("Reader_ACT(2).csv", cum_trans, latency);
    read_csv("Reader_ACT(3).csv", cum_trans, latency);
    read_csv("Reader_WGT(0).csv", cum_trans, latency);
    read_csv("Reader_WGT(1).csv", cum_trans, latency);
    read_csv("Reader_WGT(2).csv", cum_trans, latency);
    read_csv("Reader_WGT(3).csv", cum_trans, latency);

    std::map<uint32_t, uint32_t> cummulative_trans_count;

    uint32_t sum = 0;
    uint32_t min_cycle = ~0;
    uint32_t max_cycle = 0;

    for (std::map<uint32_t, uint32_t>::iterator it = cum_trans.begin();
        it != cum_trans.end(); it++)
    {
        sum += it->second;
        cummulative_trans_count[it->first] = sum;

        if (it->first < min_cycle)
            min_cycle = it->first;

        if (it->first > max_cycle)
            max_cycle = it->first;
    }

    sum = 0;

    for (uint32_t c = 0; c <= max_cycle; ++c) {
        std::map<uint32_t, uint32_t>::iterator it = cummulative_trans_count.find(c);

        if(it == cummulative_trans_count.end())
            cummulative_trans_count[c] = sum;

        sum = cummulative_trans_count[c];
    }

    std::ofstream cmx_reads("IDU_cummulative_CMX_reads.csv");
    cmx_reads << "clock,total_completed_transactions" << std::endl;

    for (std::map<uint32_t, uint32_t>::iterator it = cummulative_trans_count.begin();
        it != cummulative_trans_count.end(); it++)
    {
        cmx_reads << it->first << "," << it->second << std::endl;
    }

    cmx_reads.close();


    std::ofstream cmx_lat("IDU_CMX_read_latency.csv");
    cmx_lat << "latency,frequency" << std::endl;

    for (std::map<uint32_t, uint32_t>::iterator it = latency.begin();
        it != latency.end(); it++)
    {
        cmx_lat << it->first << "," << it->second << std::endl;
    }

    cmx_lat.close();

    return 0;
}

bool read_csv(
    const std::string &filename, 
    std::map<uint32_t, uint32_t> &cum_trans,
    std::map<uint32_t, uint32_t> &latency)
{
    std::ifstream csv_file(filename.c_str());

    std::string line_in;    

    if (!csv_file.good())
        return false;
    uint32_t line_no = 0;
    uint32_t clock_begin = 0;
    uint32_t clock_end = 0;
    uint32_t entry = 0;
    uint32_t lat = 0;

    while (!csv_file.eof()) {
        getline(csv_file, line_in);
        std::istringstream ss(line_in);
        std::string token;

        line_no++;

        if (line_no > 1) { // skip header (clock_begin,clock_end,stream,channel,type)
            entry = 0;
            while (std::getline(ss, token, ',')) {
                if (!token.length())
                    continue;
                
                entry++;

                switch (entry)
                {
                case 1:
                    clock_begin = static_cast<uint32_t>(atoi(token.c_str()));
                    break;
                case 2:
                    clock_end = static_cast<uint32_t>(atoi(token.c_str()));
                    lat = clock_end - clock_begin;
                    cum_trans[clock_end]++;
                    latency[lat]++;
                    break;
                }
            }
        }
    }
}


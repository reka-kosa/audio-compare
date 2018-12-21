#include <boost/filesystem.hpp>

#include "InitProcessing.h"

InitProcessing::InitProcessing()
{
    _sampling_rate = 0;
    _src_folder = "";
}

const std::string& InitProcessing::get_src_folder() const { return _src_folder; }
unsigned int InitProcessing::get_sampling_rate() const { return _sampling_rate; }

void InitProcessing::InteractWithUser()
{
    std::string sampling_rate;
    int sr;
    std::string path;

    std::cout << "Path to music folder: ";
    std::getline(std::cin, path);
    std::cout << std::endl;

    // If no '/' at the end, add one
    if(&path.back() != "/") path += "/";
    std::cout << "Selected path: " << path << std::endl;

    std::cout << "Sampling rate? (44100 or 22050): ";
    std::getline(std::cin, sampling_rate);
    std::cout << std::endl;
    try {
        sr = std::stoi(sampling_rate);
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Invalid argument, try again!" << std::endl;
        sr = 0;
    }


    // Correct sampling rate?
    while( sr != 44100 && sr != 22050)
    {
        std::cerr << "Sampling rate must be either '44100' or '22050' Hz! Please type one of them." << std::endl;
        std::cout << "Sampling rate? (44100 or 22050): ";
        std::getline(std::cin, sampling_rate);
        std::cout << std::endl;

        try {
            sr = std::stoi(sampling_rate);
        } catch (const std::invalid_argument& e ) {
            std::cerr << e.what() << std::endl;
            std::cerr << "Invalid argument, try it again!" << std::endl;
            sr = 0;
        }
    }

    _src_folder = path;
    _sampling_rate = sr;
}

void InitProcessing::CheckDirectory()
{
    if (boost::filesystem::exists(_src_folder) && boost::filesystem::is_directory(_src_folder))
    {
        if (boost::filesystem::is_empty(_src_folder))
        {
            std::cerr << "Empty directory " << _src_folder << std::endl;
            throw std::invalid_argument("Problem with given directory: exists but empty. Fill it with .mp3 files!");
        }
    }
    else
    {
        std::cerr << "No such directory " << _src_folder << std::endl;
        throw std::invalid_argument("Problem with given directory. Check and try again");
    }
}



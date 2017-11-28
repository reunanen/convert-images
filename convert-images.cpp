#include <dlib/dir_nav/dir_nav_extensions.h>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "cxxopts/include/cxxopts.hpp"

int main(int argc, char** argv)
{
    if (argc == 1) {
        std::cout << "Usage: " << std::endl;
        std::cout << "> convert-images -i=/path/to/input -o=/path/to/output -f=.png" << std::endl;
        return 1;
    }

    cxxopts::Options options("convert-images", "Batch-convert images");

    options.add_options()
        ("i,input-directory", "The directory where to search for input files", cxxopts::value<std::string>())
        ("o,output-directory", "The directory where to write the output files", cxxopts::value<std::string>())
        ("f,output-format", "The output image format (for example, .png)", cxxopts::value<std::string>())
        ;

    try {
        options.parse(argc, argv);

        cxxopts::check_required(options, { "input-directory", "output-directory", "output-format" });

        const std::string input_directory = options["input-directory"].as<std::string>();
        const std::string output_directory = options["output-directory"].as<std::string>();
        const std::string output_format = options["output-format"].as<std::string>();

        std::cout << "Searching for images in " << input_directory << " ..." << std::endl;

        const std::vector<dlib::file> files = dlib::get_files_in_directory_tree(input_directory, dlib::match_endings(".jpeg .jpg .png .BMP"));

        std::cout << "Found " << files.size() << " files, now converting ..." << std::endl;

        for (const auto& file : files) {

            const std::string& full_name = file.full_name();

            std::cout << "Processing " << full_name;

            cv::Mat image = cv::imread(full_name, cv::IMREAD_UNCHANGED);

            if (!image.data) {
                std::cout << " - unable to read, skipping...";
            }
            else {
                std::cout
                    << ", width = " << image.cols
                    << ", height = " << image.rows
                    << ", channels = " << image.channels()
                    << ", type = 0x" << std::hex << image.type();

                const auto name = file.name();
                const auto dot_pos = name.find('.');
                const auto output_name = output_directory + name.substr(0, dot_pos) + output_format;

                cv::imwrite(output_name, image);
            }

            std::cout << std::endl;
        }

        return 0;
    }
    catch (std::exception& e) {
        std::cerr << std::endl << "Error: " << e.what() << std::endl;
        std::cerr << std::endl << options.help() << std::endl;
        return 255;
    }
}

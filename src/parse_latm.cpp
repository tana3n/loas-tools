#include <filesystem>
//•¨Ž©‘Ì‚Ílatm_decoder.cpp‚©‚ç‚Ì—¬—p
int GetChannelConfiguration(const char* input) {
    //int channelConfigurationIndex = ((input[3] >> 3) & 0xF);// | (input[2]) + 3);
    std::uintmax_t channelConfigurationIndex = ((input[6] >> 3) & 0xF);// | (input[2]) + 3);

    return channelConfigurationIndex;
}
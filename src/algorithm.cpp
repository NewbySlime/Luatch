#include "algorithm.h"


const char* _base64_char_table =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789+/"
;

const char _base64_padding = '=';


// returns -1 if not found
static int _get_base64_index(char c){
  for(int i = 0; _base64_char_table[i]; i++){
    if(c == _base64_char_table[i])
      return i;

    if(c == _base64_padding)
      return 0;
  }

  return -1;
}

// only tree bytes (24 bit) will be used
static std::string _as_base64_string(uint32_t data, int padding = 0){
  std::string _res;
  for(int i = 3; i >= padding; i--){
    uint8_t _index = (data >> (i*6)) & 0b111111;
    _res += _base64_char_table[_index];
  }

  return _res + std::string(padding, _base64_padding);
}


std::string base64_encode(const void* buffer, size_t buflen){
  const char* _cbuf = (const char*)buffer;
  std::string _res;

  size_t _ibuflen = buflen/3;
  for(size_t i = 0; i < _ibuflen; i++){
    uint32_t _data =
      _cbuf[i*3] << 16 |
      _cbuf[i*3+1] << 8 |
      _cbuf[i*3+2]
    ;

    _res += _as_base64_string(_data);
  }

  size_t _buflen_remainder = buflen%3;
  if(_buflen_remainder > 0){
    uint32_t _data = 0;
    for(size_t i = 0; i < _buflen_remainder; i++)
      _data |= _cbuf[_ibuflen*3 + i] << ((3-(i+1)) * 8);

    _res += _as_base64_string(_data, (3-_buflen_remainder));
  }

  return _res;
}


bool base64_decode(const std::string& data, void* buffer, size_t buflen){
  char* _cbuf = (char*)buffer;
  
  // remainder (not padded) ignored
  size_t _iter_len = data.size() / 4;
  for(int i = 0; i < _iter_len; i++){
    uint32_t _data = 0;
    // parse data
    for(int si = 0; si < 4; si++){
      int _idx = _get_base64_index(data[i*4 + si]);
      // error occurred
      if(_idx < 0)
        return false;

      _data |= _idx << (18 - (si*6));
    }

    // check for padding
    for(int si = 0; si < 4; si++){
      char _c = data[i*4 + si];
      
      // prompt finish. padding found
      if(_c == '='){
        i = _iter_len;
        break;
      }

      if(si < 1)
        continue;

      // prompt finish. buffer not sufficient
      if((i*3 + si-1) >= buflen){
        i = _iter_len;
        break;
      }

      _cbuf[i*3 + si-1] = (_data >> ((2-(si-1)) * 8)) & 0xFF;
    }
  }

  return true;
}


size_t base64_get_decode_length(const std::string& data){
  // remainder (not padded) ignored
  size_t _len = data.size() / 4 * 3;
  int _occurrence = 0;
  for(size_t i = data.size() - (data.size()%4) - 1; i >= 0; i--){
    char _c = data[i];
    if(_c != '=')
      break;

    _occurrence++;
    _len--;

    // skips to next uint32_t if occurrence more than 2 (empty data)
    if(_occurrence > 2)
      i--;
  }

  return _len;
}
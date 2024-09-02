#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <vector>

enum TokenType {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOUBLE_QUOTE,
  COLON,

  STRING,
  NUMBER,
  BOOL,
  NIL,
  END_OF_FILE,
};

class Token {
  TokenType type;
  int line;
  std::string lexeme;
  std::string literal;

public:
  Token(TokenType type, std::string lexeme, std::string literal, int line) {
    this->type = type;
    this->line = line;
    this->lexeme = lexeme;
    this->literal = literal;
  };

  std::string to_string() {
    return "Token<" + std::to_string(this->type) + ", " + this->lexeme + ", " +
           this->literal + ", " + std::to_string(this->line) + ">";
  }
};

void report(int line, std::string where, std::string message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
}

void error(int line, std::string message) { report(line, "", message); }

class Scanner {
private:
  std::vector<Token> tokens;
  std::istream &source;

  int start = 0;
  int current = 0;
  int line = 1;

  void add_token(TokenType type) { add_token(type, ""); }

  void add_token(TokenType type, std::string literal) {
    std::string lexeme = "";
    Token token(type, lexeme, literal, line);
    tokens.push_back(token);
  }

  char advance() {
    char c = peek();

    this->current++;
    return c;
  }

  char peek() {
    if (is_at_end()) {
      return '\0';
    }

    char c;
    this->source.seekg(current);
    this->source.get(c);

    return c;
  }

  bool is_at_end() {
    this->source.seekg(current);
    // char c;
    // this->source.get(c);
    return source.peek() == EOF;
    // return c == EOF;
    // bool is_eof = this->source.eof();
    //
    // return is_eof;
  }

  void string() {
    while (peek() != '"' && !is_at_end()) {
      if (peek() == '\n') {
        line++;
      }
      advance();
    }

    if (is_at_end()) {
      error(line, "Unterminated string.");
      return;
    }

    advance();

    std::string str_content;

    source.seekg(start + 1);
    char crr_char;
    while (source.get(crr_char) && source.tellg() < current) {
      str_content.push_back(crr_char);
    }

    add_token(STRING, str_content);
  }


public:
  explicit Scanner(std::istream &source) : source(source) {}

  std::vector<Token> scan_tokens() {
    while (!is_at_end()) {
      start = current;
      scan_token();
    }

    add_token(END_OF_FILE);

    return this->tokens;
  }

  void scan_token() {
    // while we have things coming in keep looping
    char c = advance();
    switch (c) {
    case '{':
      add_token(LEFT_BRACE);
      break;
    case '}':
      add_token(RIGHT_BRACE);
      break;
    case '[':
      add_token(LEFT_PAREN);
      break;
    case ']':
      add_token(RIGHT_PAREN);
      break;
    case ',':
      add_token(COMMA);
      break;
    case '"':
      string();
      break;
    case ':':
      add_token(COLON);
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line++;
      break;
      default:
      error(this->line, "Unexpected character.");
      break;
    }
  }
};

void run(std::istream &stream) {
  // stream.seekg(18);
  //
  // char c;
  // stream.get(c);
  //
  // bool is_eof = c == std::istream::eofbit;
  // bool is_eof_2 = stream.eof();
  //
  // std::cout << "Read: " << c << "\t" << is_eof << "\t" << is_eof_2 <<
  // std::endl;

  Scanner s(stream);

  std::vector<Token> tokens = s.scan_tokens();

  for (Token token : tokens) {
    std::cout << token.to_string() << std::endl;
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
    exit(64);
  }

  std::string file_path = argv[1];
  std::ifstream input_stream(file_path);
  if (!input_stream || !input_stream.is_open()) {
    std::cerr << "Could not open " << file_path << " for reading!" << std::endl;
    exit(65);
  }

  // char c;
  // input_stream.seekg(19);
  // input_stream.get(c);
  //
  // bool is_eof = input_stream.eof();
  //
  // if (is_eof) {
  //   std::cout << "IM EOF" << std::endl;
  // }
  //
  // std::cout << "INPUT STREAM EOF: " << c << "\t is it eof?: " << is_eof << std::endl;

  run(input_stream);
}
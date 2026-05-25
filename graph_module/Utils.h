#ifndef UTILS_H
#define UTILS_H

#include <cstddef>
#include <string>

using Identifier = size_t;

class Exception {
public:
  virtual ~Exception() = default;
  virtual const char* message() const = 0;
};

class MemoryException : public Exception {
public:
  MemoryException(const char* msg) : msg_(msg) {}

  const char* message() const override { return msg_; }

private:
  const char* msg_;
};

class VariableException : public Exception {
public:
  VariableException(std::string msg) : msg_(std::move(msg)) {}

  const char* message() const override { return msg_.c_str(); }

protected:
  void replace(const std::string& placeholder, const std::string& value) {
    size_t pos = msg_.find(placeholder);
    if (pos != std::string::npos) {
      msg_.replace(pos, placeholder.length(), value);
    }
  }
  std::string msg_;
};

class IdentifierException : public VariableException {
public:
  IdentifierException(const std::string& templateMsg, Identifier id) : VariableException(templateMsg) {
    replace("$id", std::to_string(id));
  }
};

class ElementException : public VariableException {
public:
  ElementException(const std::string& templateMsg, Identifier id) : VariableException(templateMsg) {
    replace("$id", std::to_string(id));
  }
  ElementException(const std::string& templateMsg, Identifier source, Identifier target)
      : VariableException(templateMsg) {
    replace("$source", std::to_string(source));
    replace("$target", std::to_string(target));
  }
};

class ConflictException : public VariableException {
public:
  ConflictException(const std::string& templateMsg, Identifier id) : VariableException(templateMsg) {
    replace("$id", std::to_string(id));
  }

  ConflictException(const std::string& templateMsg, Identifier source, Identifier target)
      : VariableException(templateMsg) {
    replace("$source", std::to_string(source));
    replace("$target", std::to_string(target));
  }
};

class FileException : public VariableException {
public:
  FileException(const std::string& templateMsg, const std::string_view& filename)
      : VariableException(templateMsg) {
    replace("$filename", filename.data());
  }
};

const char UNAVAILABLE_MEMORY_NODE[] = "Unavailable memory for a new node insertion";
const char UNAVAILABLE_MEMORY_EDGE[] = "Unavailable memory for a new edge insertion";
const char INVALID_NODE_IDENTIFIER[] = "Invalid node identifier $id requested";
const char INVALID_EDGE_IDENTIFIER[] = "Invalid edge identifier $id requested";
const char NODE_ALREADY_EXISTS[] = "Node with identifier $id already exists";
const char ID_EDGE_ALREADY_EXISTS[] = "Edge with identifier $id already exists";
const char NON_SUCCESSIVE_NODE[] = "Non-successive node identifier $id requested";
const char NON_SUCCESSIVE_EDGE[] = "Non-successive edge identifier $id requested";
const char SOURCE_NOT_EXIST[] = "Source node with identifier $id does not exist";
const char TARGET_NOT_EXIST[] = "Target node with identifier $id does not exist";
const char EDGE_ALREADY_EXISTS[] = "Edge between nodes $source and $target already exists";
const char EDGE_BETWEEN_SRC_TRGT_NOT_EXIST[] = "Edge between nodes $source and $target does not exist";
const char UNABLE_TO_OPEN_INPUT_FILE[] = "Unable to open input file $filename";
const char UNABLE_TO_OPEN_OUTPUT_FILE[] = "Unable to open output file $filename";
const char NODE_DOES_NOT_EXIST[] = "Node with identifier $id does not exist";
const char EDGE_DOES_NOT_EXIST[] = "Edge with identifier $id does not exist";
const char UNAVAILABLE_MEMORY_IMPORT[] = "Unavailable memory for importing graph content";
const char UNAVAILABLE_MEMORY_COPY[] = "Unavailable memory for creating a graph copy";
const char UNAVAILABLE_MEMORY_ASSIGN[] = "Unavailable memory for assigning a graph copy";

#endif

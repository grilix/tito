#pragma once

#include <cstring>
#include <cstdint>

#include "interfaces.h"

#define TITO_MAX_TOPIC_LEN 50

namespace tito {
  class TopicNode : public TopicBuilder {
    public:

      /* Create tree branch.
      *
      * Maximum total topic length is 255.
      *
      * Example:
      *
      *    TopicNode *root = new TopicNode(nullptr, "root");
      *    TopicNode *branch1 = new TopicNode(root, "branch1"); // -> root/branch1
      *    TopicNode *branch2 = new TopicNode(root, "branch2"); // -> root/branch2
      *
      */
      TopicNode(const TopicNode* parent, const char* text) :
        totalLen(strlen(text)),
        _text(text),
        _parent(parent)
      {
        if (!parent) {
          pathLen = 0;
          return;
        }

        pathLen = parent->totalLen + 1;
        totalLen += pathLen; // extra space for "/"
      }

      explicit TopicNode(const char* text)
        : TopicNode(nullptr, text)
      { }

      /* Collapses tree path into a null terminated string.
      *
      * Example:
      *
      *    char* buffer[100];
      *    if (node->buildTopic(buffer, 100, "state") == 0) {
      *      return; // handle error
      *    }
      *    printf("%s\n", buffer); // -> "root/branch2/state"
      */
      size_t buildTopic(char buffer[], const size_t maxLen, const char* topic) const override
      {
        /* Copies each part from right to left, adding a "/" between each one.
         */
        const TopicNode* cur = this;

        size_t partLen = strlen(topic),
               fullLen = cur->totalLen + 1 + partLen;

        if (partLen == 0) {
          return 0;
        }

        if (maxLen < (fullLen + 1)) {
          return 0;
        }

        char* pos = buffer + fullLen;

        pos -= partLen;
        memcpy(pos, topic, partLen + 1); // also copy \0
        *--pos = '/';

        do {
          partLen = cur->totalLen - cur->pathLen;

          pos -= partLen;
          memcpy(pos, cur->_text, partLen);

          if (cur->_parent != nullptr) {
            *--pos = '/';
          }
        } while ((cur = cur->_parent) != nullptr);

        return fullLen;
      }

      bool topicMatches(const char* topic, size_t len) const
      {
        if (len != totalLen) {
          return false;
        }

        const TopicNode* cur = this;

        const char* pos = topic + len + 1;

        do {
          size_t partLen;

          partLen = cur->totalLen - cur->pathLen;
          pos -= partLen + 1;

          if (strncmp(pos, cur->_text, partLen) != 0) {
            return false;
          }
        } while ((cur = cur->_parent) != nullptr);

        return true;
      }

      size_t getTotalLen() const { return totalLen; }
      const char* getText() const { return _text; }

    private:
      size_t totalLen = 0;
      size_t pathLen;

      const char* _text;
      const TopicNode* _parent;
  };
};

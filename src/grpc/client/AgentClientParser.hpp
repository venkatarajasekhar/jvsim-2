//
//  AgentClientParser.hpp
//  agent-jv
//
//  Created by NITIN KUMAR on 1/22/16.
//  Copyright © 2016 Juniper Networks. All rights reserved.
//

#ifndef AgentClientParser_hpp
#define AgentClientParser_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

#define MAX_BUFFER 100
#define MAX_ARGS   10

typedef void (*CmdHandler)(int argc, const char *argv[]);
typedef struct entry_ {
    std::string e_cmd;
    uint32_t    e_argc;
    std::string e_help;
    std::string e_usage;
    CmdHandler  e_handler;
} entry_t;
extern entry_t  agent_client_commands[];
extern uint32_t agent_client_commands_count;

// Container to pass arguments to handler thread
class CommandContext {
    int          argument_count;
    const char  *arguments[40];
    entry_t     *entry;
    
public:
    int           getArgc ()  { return argument_count; }
    const char ** getArgv ()  { return arguments; }
    entry_t      *getEntry () { return entry; }
    
    CommandContext (int argc, const char *argv[], entry_t *e)
    {
        int i;
        
        for (i = 0; i < argc; i++) {
            std::string *ptr = new std::string(argv[i]);
            arguments[i] = ptr->c_str();
        }
        arguments[i] = NULL;
        argument_count = argc;
        entry = e;
    }
   
};

// A simple interpreter
class AgentParser {
    std::map<std::string, entry_t *> _grammar;
    
public:
    AgentParser ()
    {
        for (uint32_t i = 0; i < agent_client_commands_count; i++) {
            entry_t *entry = agent_client_commands + i;
            _grammar[entry->e_cmd] = entry;
        }
    }
    
    entry_t *lookupCmd (const char *cmd)
    {
        if (_grammar.count(cmd) == 0) {
            return NULL;
        }
        
        return _grammar[cmd];
    }
    
    void help ()
    {
        std::map<std::string, entry_t *>::iterator itr;
        
        for (itr = _grammar.begin(); itr != _grammar.end(); itr++) {
            std::cout << itr->first << ":\n";
            std::cout << "   " << itr->second->e_help << "\n";
            std::cout << "   " << itr->second->e_usage << "\n\n";
        }
    }
    
    void clean(const char *cmd[])
    {
        for (int a=0; a < 40; a++) {
            cmd[a] = NULL;
        }
    }
    
    int parseArg(char* cnd, const char* cmd[], char input[])
    {
        std::cout << "jvsim> ";
        std::cin.getline(input,50);
        cnd = strtok(input, " ");
        
        int i = 0;
        while (cnd != NULL) {
            cmd[i] = cnd;
            i++;
            cnd = strtok(NULL, " ");
        }
        
        return i;
    }
    
    static void *executeCallback (void *rock)
    {
        CommandContext *context = (CommandContext *) rock;
        
        context->getEntry()->e_handler(context->getArgc(), context->getArgv());
        delete context;
        return NULL;
    }
    
    void execute(int argc, const char* argv[])
    {
        // Find the command
        entry_t *e = lookupCmd(argv[0]);
        if (!e) {
            std::cout << "Unrecognised command\n";
            return;
        }
        
        // Does the argument count match
        if (argc < e->e_argc) {
            std::cout << "Missing arguments:Usage:" << e->e_help << "\n";
            return;
        }
        
        // Create a context for the handler
        CommandContext *context = new CommandContext(argc, argv, e);
        
        // create a thread that invokes the handler
        pthread_t pthread;
        pthread_create(&pthread, NULL, executeCallback, (void *)context);
    }
    
    void parseLoop ()
    {
        char* cnd;
        const char* cmd[40];
        char input[50];
        int  argc;
        
        do {
            clean(cmd);
            argc = parseArg(cnd, cmd, input);
            if (argc == 0) {
                continue;
            }
            
            // Is the poor soul asking for help ? Give him some succour
            if (strcmp(cmd[0], "help") == 0 ||
                strcmp(cmd[0], "?") == 0) {
                help();
                continue;
            }
            
            // Are we done ?
            if(strcmp(cmd[0], "exit") == 0 || strcmp(cmd[0], "quit") == 0 ) {
                break;
            }
            execute(argc, cmd);
        } while (true);
        
        return;
    }
    
};

#endif /* AgentClientParser_hpp */
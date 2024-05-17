#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
/*************************************************************************************/
//main functions
void write_to_log_file(char *str);
int reap_child_zombie();
void on_child_exit();
int check_for_$(char *str);
int execute_shell_bultin(char **args);
void execute_command(char **args, int flag);
char **parse_input(char *str);
void print_path();
void shell();
char** export_command(char* str);
void setup_environment();
void clear_log_file();
/**************************************************************************************/
//global variables
char **variables;
char **values;
int indx;
int flag;
/**************************************************************************************/
int main() {
 setup_environment();
 shell();
  return 0;
}
/**************************************************************************************/
int reap_child_zombie() {
  int pid = 0;
  char *pid_str = (char *)malloc(100 * sizeof(char));
  char *pid_str_temp = (char *)malloc(100 * sizeof(char));

  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
    strcpy(pid_str, "Child process with PID ");
    sprintf(pid_str_temp, "%d", pid);
    strcat(pid_str, pid_str_temp);
    strcat(pid_str, " terminated (Zombie).\n");
    write_to_log_file(pid_str);
    return 1;
  }
  return 0;
}
void on_child_exit() {
  if (!reap_child_zombie())
    write_to_log_file("Child terminated\n");
}

/**************************************************************************************/

int check_for_$(char *str) {
  char **token = malloc(100 * sizeof(char *));
  char *str1 = malloc(100 * sizeof(char *));
  token[0] = strtok(str, "$");
  int i = 0;
  while (token[i] != NULL) {
    i++;
    token[i] = strtok(NULL, "$");
  }

  for (int j = 0; j < i; j++) {

    if (j == 0 && str[0] != '$') {
      strcat(str1, token[j]);
      continue;
    }
    int flagy = 0;
    for (int k = 0; k < indx; k++) {

      if (strcmp(variables[k], token[j]) == 0) {
        strcat(str1, values[k]);
        flagy = 1;
      }
    }
    if (flagy != 1)
      break;
  }

  strcpy(str, str1);
  return 1;
}
/***********************************************************************************/

int execute_shell_bultin(char **args) {
  if (strcmp(*args, "cd") == 0) {
    if (args[1] != NULL)
      chdir(args[1]);
    return 1;
  } else if (strcmp(*args, "export") == 0) {
    int i = 0;
    while (args[i] != NULL) {
      i++;
    }

    int j = 1;
    while (args[j] != NULL) {
      int flagy = 0;
      char *variable = (char *)malloc(100 * sizeof(char));
      variable = strtok(args[j], "=");
      for (int k = 0; k < indx; k++) {
        if (strcmp(variable, variables[k]) == 0) {
          flagy = 1;
          strcpy(values[k], strtok(NULL, "="));
          if(values[k][0]=='"')values[k]+=1;
          if(values[k][strlen(values[k])-1]=='"')values[k][strlen(values[k])-1]='\0';


        }
      }

      if (flagy == 0) {
        variables[indx] = (char *)malloc(100 * sizeof(char));
        strcat(variables[indx], variable);
        values[indx] = (char *)malloc(100 * sizeof(char));
        strcat(values[indx], strtok(NULL, "="));
        if(values[indx][0]=='"')values[indx]+=1;
        if(values[indx][strlen(values[indx])-1]=='"')values[indx][strlen(values[indx])-1]='\0';
        check_for_$(values[indx]);
        indx++;
      }
      j++;
    }

    return 1;
  } else if (strcmp(*args, "echo") == 0) {
    for (int i = 1; args[i] != NULL; i++) {
      check_for_$(args[i]);
     printf("%s ", args[i]);
      // puts(args[i]); 
    }
    printf("\n");
    return 1;
  }
  return 0;
}

/**********************************************************************************************************/
void execute_command(char **args, int flag) {

  int pid = fork();
  if (pid < 0) {
    perror("fork");
  }
  if (pid == 0) {
    execvp(args[0], args);
    perror("execvp");
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    if (!flag)
      waitpid(pid, NULL, 0);
  }

  signal(SIGCHLD, on_child_exit);
}

/**********************************************************************************************************/
char **parse_input(char *str) {

  if (strlen(str) > 0 && str[strlen(str) - 1] == '\n')
    str[strlen(str) - 1] = '\0';
  if (strlen(str) > 0 && str[strlen(str) - 1] == '&') {
    flag = 1;
   str[strlen(str) - 2] = '\0';
  }
  char* temp = (char*)malloc(100 * sizeof(char));
  strcpy(temp,str);
  char **token = malloc(100 * sizeof(char *));
  const char delimiter[2] = " ";
  char *ptr = strstr(str, "export");
  if(ptr==NULL)
  {
      token[0] = strtok(str, delimiter);
  int  i = 0;

  while (token[i] != NULL){
    i++;
    token[i] = strtok(NULL, delimiter);

  }
  i = 1;
  while (token[i] != NULL) {
    check_for_$(token[i]);
    i++;
  }
  if(i==2 &&token[1]!=NULL&& strstr(token[1], " ")!=NULL )
  {
     token[1] = strtok(token[1], delimiter);
     i = 1;
    while (token[i] != NULL){
    i++;
    token[i] = strtok(NULL, delimiter);
    }
  }
  }
 if (ptr!=NULL)
  {
    return export_command( str) ;
  }
   

  return token;
}
/**********************************************************************************************************/
char** export_command(char* str)
{
 int inside_quotes = 0;
    char *start = str;
    char *end = str;
    int count = 0;
    
    // Count the number of tokens
    while (*end != '\0') {
        if (*end == ' ' && !inside_quotes) {
            count++;
        } else if (*end == '"') {
            inside_quotes = !inside_quotes;
        }
        end++;
    }
    count++; // Increment for the last token
    
    // Allocate memory for token array
    char **tokens = (char**)malloc(count * sizeof(char*));
    
    // Loop through the string
    inside_quotes = 0; // Reset inside_quotes flag
    end = str; // Reset end pointer
    int i = 0;
    while (*end != '\0') {
        if (*end == '"') {
            inside_quotes = !inside_quotes;
        }
        // Check if the current character is a space and not inside quotes
        else if (*end == ' ' && !inside_quotes) {
            *end = '\0'; // Replace space with null terminator to split the string
            // Allocate memory for token and copy the token
            tokens[i] = (char*)malloc((end - start + 1) * sizeof(char));
            strcpy(tokens[i], start);
            // Move start pointer to the next character
            start = end + 1;
            i++;
        }
        // Move to the next character
        end++;
    }
    // Copy the last token
    tokens[i] = (char*)malloc((end - start + 1) * sizeof(char));
    strcpy(tokens[i], start);    
    return tokens;
}
void print_path()
{
  char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) ;
    printf("\033[32m"); // Change text color to green
    printf("\033[1m");  // Make text bold
    printf("habiba@habiba-vostro-3500:");
    printf("\033[34m"); // Change text color to blue
    printf("\033[1m");  // Make text bold
    printf("%s",cwd);
    printf("\033[0m");  // Reset text color to default
    printf("$");
}
/**********************************************************************************************************/

void shell()
{
   variables = malloc(100 * sizeof(char *));
  values = malloc(100 * sizeof(char *));
  char str[50];
   do {
    
    print_path();
    flag = 0;
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = '\0';

    if (str == NULL || strcmp(str, "\n") == 0)
      continue;

    char **token;
    token = parse_input(str);

    if (strcmp(str, "exit") == 0)
      exit(0);

    if (execute_shell_bultin(token)) {
      continue;
    }

    execute_command(token, flag);

  } while (strcmp(str, "exit"));

}
void setup_environment()
{
      execute_command("clear", flag);
      clear_log_file();

}
void clear_log_file() 
{
    FILE *file;
    file = fopen("/home/habiba/habibatarek", "w+"); // "w+" mode truncates the file if it exists, otherwise creates a new file
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fclose(file);
}
void write_to_log_file(char *str) {
  FILE *log_file;
  log_file =
      fopen("/home/habiba/habibatarek", "a"); // Open log file in append mode

  if (log_file == NULL) {
    perror("Error opening log file");
    exit(EXIT_FAILURE);
  }

  fprintf(log_file, "%s", str);

  fclose(log_file);
}

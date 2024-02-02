#include <fcntl.h>
#include "fonctions.h"

int main(){
    int pid, status;
    char argument[BUFSIZ], truc[BUFSIZ], time[BUFSIZ];
    struct timespec start, stop;

    write(STDOUT_FILENO, welcome, strlen(welcome));

    while (1){
        if (strlen(truc) == 0){
            write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
        } else{
            write(STDOUT_FILENO, "enseash ", strlen("enseash "));
            write(STDOUT_FILENO, truc, strlen(truc));
            write(STDOUT_FILENO, time, strlen(time));
            write(STDOUT_FILENO, "] % ", strlen("] % "));
        }
        read(STDIN_FILENO, argument, BUFSIZ);
        argument[strcspn(argument, "\n")] = 0;


        /** Comparaison avec les 4 premières caractères et que la longueur de la chaîne de caractère vaut 4 **/
        if ((strncmp(argument, "exit", 4) == 0) & (strlen(argument) == 4)){
            write(STDOUT_FILENO, end, strlen(end));
            return EXIT_SUCCESS;
        }

        /** Démarrer l'horloge **/
        if (clock_gettime(CLOCK_REALTIME, &start) == -1){
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }


        pid = fork();
        if (pid == 0){
            // Diviser l'argument en commande et en arguments
            char* args[BUFSIZ];
            char* token = strtok(argument, " ");
            int i = 0, fd_in, fd_out;
            while (token != NULL) {
                // Vérifier la direction de l'entrée
                if (strcmp(token, "<") == 0) {
                    token = strtok(NULL, " ");
                    if ((fd_in = open(token, O_RDONLY)) < 0) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    if (dup2(fd_in, STDIN_FILENO) < 0) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(fd_in);
                } else if (strcmp(token, ">") == 0) {
                    token = strtok(NULL, " ");
                    if ((fd_out = open(token, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) {
                        perror("open");
                        exit(EXIT_FAILURE);
                    }
                    if (dup2(fd_out, STDOUT_FILENO) < 0) {
                        perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(fd_out);
                } else {
                    args[i] = token;
                    i++;
                }
                token = strtok(NULL, " ");
            }
            args[i] = NULL;  // End of arguments

            execvp(args[0], args);
            write(STDOUT_FILENO, "loser ça existe pô\n", strlen("loser ça existe pô\n"));
            exit(-1);
        } else{
            while (-1 != (pid = wait(&status))){
                if (WIFEXITED(status)){
                    sprintf(truc, "[exit:%d", WEXITSTATUS(status));

                } else if (WIFSIGNALED(status)){
                    sprintf(truc, "[sign:%d", WTERMSIG(status));
                }
            }
        }

        /** Arrêter l'horloge **/
        if (clock_gettime(CLOCK_REALTIME, &stop) == -1){
            perror("clock gettime");
            exit(EXIT_FAILURE);
        }

        int durationSeconds = (int) (stop.tv_sec - start.tv_sec);
        if (durationSeconds >= 60){
            sprintf(time, "|%d min %ds", durationSeconds / 60, durationSeconds % 60);
        } else if (durationSeconds > 0){
            sprintf(time, "|%ds", durationSeconds);
        } else{
            sprintf(time, "|%dms", (int) ((int) (stop.tv_nsec - start.tv_nsec) * 10e-6));
        }
    }
}

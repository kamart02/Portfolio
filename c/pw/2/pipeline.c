#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"
#include "pipeline-utils.h"

int main(int argc, char* argv[]) {
   if (argc < 2) {
      return 1;
   }

   int last_fd[2] = {STDIN_FILENO, STDOUT_FILENO};
   int fd[2];

   print_open_descriptors();
   fprintf(stderr, "\n");

   for (int i = 1; i < argc; i++) {
      
      ASSERT_SYS_OK(pipe(fd));

      pid_t pid = fork();
      ASSERT_SYS_OK(pid);

      if (!pid) {
         //child
         if (i > 1) {
            // Change stdin of process to lastPipe read
            ASSERT_SYS_OK(dup2(last_fd[0], STDIN_FILENO));
         }
         if (i < argc - 1) {
            // Close not needed read pipe
            ASSERT_SYS_OK(close(fd[0]));
            // Change stdout of process to lastPipe write
            ASSERT_SYS_OK(dup2(fd[1], STDOUT_FILENO));
         }


         // Print open decriptior
         print_open_descriptors();
         fprintf(stderr, "\n");
         // Execute the program
         ASSERT_SYS_OK(execlp(argv[i], argv[i], NULL));
      }

      sleep(1);

      // Close previous write pipe
      if (i > 1) {
         ASSERT_SYS_OK(close(last_fd[0]));
      }
      //Close not needed write pipe
      ASSERT_SYS_OK(close(fd[1]));

      // Remember last pipe
      last_fd[0] = fd[0];
      last_fd[1] = fd[1];
   }

   // After iterations remembered pipe read is not closed
//    ASSERT_SYS_OK(close(last_fd[0]));

   for (int i = 1; i < argc; ++i) {
        ASSERT_SYS_OK(wait(NULL));
   }

   ASSERT_SYS_OK(close(last_fd[0]));

   print_open_descriptors();
   fprintf(stderr, "\n");

   return 0;
}
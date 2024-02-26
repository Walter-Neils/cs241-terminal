#ifndef FORK_HELPERS_H
#define FORK_HELPERS_H

#define FORK_FAILED(forkval) (forkval < 0)
#define FORK_IS_CHILD(forkval) (forkval == 0)
#define FORK_IS_PARENT(forkval) (forkval > 0)


#endif

# Scheduler


Workers call `Scheduler::listen()`, which waits for instructions from the
controller --- which processes participate in the job, what function to call,
with what arguments, etc --- executes a given job, and then sends the results
to the controller. Note that any rank that returns a value from the Chaiscript
function that's given to the job will send that value back to the controller
(specifically, rank 0 on the `world` communicator). It's on the user to make
sure only one rank returns a result. Otherwise, only one of the answers will be
accepted, and the other ones will be logged as errors.

Controller launches jobs via `Scheduler::schedule()`, which simply places the
job arguments into a queue. The real work is performed by
`Scheduler::control()`, which must be run in a loop. It checks for finished
jobs (and retrieves their results). If there are both jobs in the queue and
available processors, the jobs get scheduled.


**TODO:**
- When workers are instructed to run a job, the controller sends point-to-point
  messages to each rank involved in the job. This needs to be replaced with
  a (manually simulated) broadcast.

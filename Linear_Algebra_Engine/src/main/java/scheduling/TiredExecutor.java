package scheduling;

import java.util.concurrent.PriorityBlockingQueue;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

public class TiredExecutor {

    private final TiredThread[] workers;
    private final PriorityBlockingQueue<TiredThread> idleMinHeap = new PriorityBlockingQueue<>();
    private final AtomicInteger inFlight = new AtomicInteger(0);

    public TiredExecutor(int numThreads) {
        // TODO
        //workers = null; // placeholder
        workers = new TiredThread[numThreads];
        for (int i = 0; i < numThreads; i++) {
            TiredThread currentThread = new TiredThread(i, 1.0);
            currentThread.start();
            workers[i] = currentThread;
            idleMinHeap.add(currentThread);
        }
    }

    public void submit(Runnable task) {
        try {
            TiredThread worker = idleMinHeap.take();
            inFlight.incrementAndGet();
            Runnable wrappedTask = () -> {
                try {
                    task.run();
                } finally {
                   idleMinHeap.add(worker);
                   int remaining = inFlight.decrementAndGet();
                   if(remaining == 0) {
                       synchronized (this) {
                           notifyAll();
                       }
                   }
                    
                }
            };
            worker.newTask(wrappedTask);

        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            // Handle any exceptions that might occur during task submission
        }
    }

    public void submitAll(Iterable<Runnable> tasks) {
        for(Runnable task : tasks) {
            submit(task);
        }
        synchronized (this) {
            while(inFlight.get() > 0) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    break;
                }
            }
        }
    }

    public void shutdown() throws InterruptedException {
        // TODO
        for (TiredThread worker : workers) {
            if (worker != null) {
                worker.shutdown();
            }
        }
        for (TiredThread worker : workers) {
            if (worker != null) {
                worker.join();
            }
        }
    }

    public synchronized String getWorkerReport() {
        // TODO: return readable statistics for each worker
        StringBuilder report = new StringBuilder();
        report.append("-------------------------------------------------------------\n");
        report.append("Executor Worker Report:\n");
        
        for (TiredThread worker : workers) {
            if (worker != null) {
                report.append("Worker ").append(worker.getWorkerId()).append(":\n");

                report.append("Fatigue: ").append(String.format(java.util.Locale.US,"%.2f", worker.getFatigue())).append("\n");
                report.append("Time Used: ").append(worker.getTimeUsed()).append("ms").append("\n");
                report.append("Time Idle: ").append(worker.getTimeIdle()).append("ms").append("\n");
                report.append("Status: ").append(worker.isBusy() ? "WORKING" : "IDLE").append("\n");
                report.append("-------------------------------------------------------------\n");
            }
        }       
        return report.toString();
    }

}

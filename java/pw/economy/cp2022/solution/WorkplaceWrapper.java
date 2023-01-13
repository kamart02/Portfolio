package cp2022.solution;

import cp2022.base.Workplace;

import java.util.concurrent.Semaphore;

public class WorkplaceWrapper extends Workplace {
    private final Workplace workplace;
    private final ConcurentWorkshop workshop;
    private WorkerStatus nextStatus = null;
    private WorkerStatus workingStatus = null;
    private WorkerStatus waitingStatus = null;
    private final Semaphore useLock = new Semaphore(1);
    private final Semaphore waitingQueue = new Semaphore(0);

    public WorkplaceWrapper(Workplace workplace, ConcurentWorkshop workshop) {
        super(workplace.getId());
        this.workplace = workplace;
        this.workshop = workshop;
    }

    public void unlockUse() {
        useLock.release();
    }

    public void unlockQueue() {
        waitingQueue.release();
    }

    public boolean isWaiting() {
        return waitingStatus != null;
    }

    public boolean isEmpty() {
        return waitingStatus == null && workingStatus == null && nextStatus == null;
    }

    public void waitInQueue() {
        try {
            workshop.mutex.release();
            waitingQueue.acquire();
            workshop.mutex.acquire();
        } catch (InterruptedException e) {
            throw new Panic();
        }
    }

    public void leave() {
        waitingStatus = null;
    }

    public void enter(WorkerStatus status) {
        nextStatus = status;
    }

    @Override
    public void use() {
        try {
            workshop.mutex.acquire();
            workingStatus = nextStatus;
            nextStatus = null;

            workingStatus.stopWaiting();

            var lastWorkplace = workingStatus.getLast();
            if (lastWorkplace != null) {
                lastWorkplace.unlockUse();
                lastWorkplace.leave();

                if (lastWorkplace != workingStatus.getCurrent()) {
                    workshop.unlockWorkplace(lastWorkplace, !workingStatus.checkCycle());
                }
            }


            workshop.mutex.release();
            useLock.acquire();

        } catch (InterruptedException e) {
            throw new Panic();
        }

        workplace.use();

        try {
            workshop.mutex.acquire();

            waitingStatus = workingStatus;
            workingStatus = null;

            workshop.mutex.release();
        } catch (InterruptedException e) {
            throw new Panic();
        }
    }

    public WorkerStatus getWaitingStatus() {
        return waitingStatus;
    }

    public WorkerStatus getNextStatus() {
        return nextStatus;
    }

    public WorkerStatus getWorkingStatus() {
        return workingStatus;
    }
}

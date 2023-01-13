package cp2022.solution;

import java.util.concurrent.Semaphore;

public class WorkerStatus {
    private final ConcurentWorkshop workshop;
    private WorkplaceWrapper last;
    private WorkplaceWrapper current;
    private WorkplaceWrapper next;
    private final long maxPatience;
    private long patience;
    private long ticket;
    private final Semaphore block = new Semaphore(0);
    private boolean wokeFromCycle = false;

    public WorkerStatus(long maxPatience, ConcurentWorkshop workshop) {
        this.maxPatience = maxPatience;
        this.workshop = workshop;
    }

    public void setCycle() {
        wokeFromCycle = true;
    }

    public boolean checkCycle() {
        boolean lastValue = wokeFromCycle;
        wokeFromCycle = false;
        return lastValue;
    }

    public void startWaiting(WorkplaceWrapper workplace, long ticket) {
        this.next = workplace;
        this.ticket = ticket;
        patience = maxPatience;
    }

    public void stopWaiting() {
        last = current;
        current = next;
        next = null;
    }

    public void block() {
        try {
            workshop.mutex.release();
            block.acquire();
            workshop.mutex.acquire();
        } catch (InterruptedException e) {
            throw new Panic();
        }
    }

    public void unBlock() {
        block.release();
    }

    public void decreasePatience() {
        this.patience--;
    }

    public WorkplaceWrapper getLast() {
        return last;
    }

    public WorkplaceWrapper getCurrent() {
        return current;
    }

    public WorkplaceWrapper getNext() {
        return next;
    }

    public long getPatience() {
        return patience;
    }

    public long getTicket() {
        return ticket;
    }

}

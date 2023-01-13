package cp2022.solution;

import cp2022.base.Workplace;
import cp2022.base.WorkplaceId;
import cp2022.base.Workshop;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Semaphore;

public class ConcurentWorkshop implements Workshop {
    public final Semaphore mutex = new Semaphore(1, true);
    private final Map<WorkplaceId, WorkplaceWrapper> workplaces = new HashMap<>();
    private final Map<Long, WorkerStatus> statuses = new HashMap<>();
    private final long maxPatience;
    private long newTicket = 0;


    public ConcurentWorkshop(Collection<Workplace> workplaces) {
        this.maxPatience = workplaces.size() * 2L - 1;
        for (var workplace: workplaces) {
            this.workplaces.put(workplace.getId(), new WorkplaceWrapper(workplace, this));
        }
    }

    public long getNewTicket() {
        return newTicket++;
    }

    public boolean canEnter(long ticket) {
        for (var status: statuses.values()) {
            if (status.getTicket() < ticket) {
                if (status.getPatience() == 0) {
                    return false;
                }
            }
        }

        return true;
    }

    public void updatePatience(long ticket) {
        for (var status: statuses.values()) {
            if (status.getTicket() < ticket) {
                status.decreasePatience();
            }
        }
    }

    @Override
    public Workplace enter(WorkplaceId wid) {
        try {
            mutex.acquire();

            var status = new WorkerStatus(maxPatience, this);
            statuses.put(Thread.currentThread().getId(), status);

            var wantedWorkplace = workplaces.get(wid);
            var ticket = getNewTicket();

            status.startWaiting(wantedWorkplace, ticket);

            if (wantedWorkplace.isEmpty()) {
                if (canEnter(ticket)) {
                    wantedWorkplace.enter(status);
                    updatePatience(ticket);
                    mutex.release();
                    return wantedWorkplace;
                }
            }
            status.block();
            mutex.release();
            return wantedWorkplace;

        } catch (InterruptedException e) {
            throw new Panic();
        }
    }

    public void unlockWorkplace(WorkplaceWrapper myWorkplace, boolean noCycle) {
        if (noCycle) {
            for (var status : statuses.values()) {
                var wantedWorkplace = status.getNext();
                var currentWorkplace = status.getCurrent();
                if (wantedWorkplace == myWorkplace) {

                    if (currentWorkplace != null) {
                        wantedWorkplace.enter(status);
                        currentWorkplace.unlockQueue();
                        break;
                    } else {
                        var ticket = status.getTicket();
                        if (canEnter(ticket)) {
                            wantedWorkplace.enter(status);
                            updatePatience(ticket);
                            status.unBlock();
                            break;
                        }
                    }
                }
            }
        }
        for (var status: statuses.values()) {
            var wantedWorkplace = status.getNext();
            var currentWorkplace = status.getCurrent();
            if (currentWorkplace == null) {
                if (wantedWorkplace != null) {
                    if (wantedWorkplace.isEmpty()) {
                        var ticket = status.getTicket();
                        if (canEnter(ticket)) {
                            wantedWorkplace.enter(status);
                            updatePatience(ticket);
                            status.unBlock();
                        }
                    }
                }
            }
        }

    }

    public boolean findAndFixCycle(WorkerStatus myStatus) {
        var myWorkplace = myStatus.getCurrent();
        var nextWorkplace = myStatus.getNext();
        ArrayList<WorkplaceWrapper> path = new ArrayList<>();

        do {
            if (nextWorkplace == null) {
                return false;
            }
            if (nextWorkplace.isEmpty()) {
                return false;
            }
            if (!nextWorkplace.isWaiting()) {
                return false;
            }
            if (nextWorkplace.getNextStatus() != null || nextWorkplace.getWorkingStatus() != null) {
                return false;
            }
            if (nextWorkplace.getWaitingStatus().getNext() == null) {
                return false;
            }
            path.add(nextWorkplace);
            nextWorkplace = nextWorkplace.getWaitingStatus().getNext();
        } while (nextWorkplace != myWorkplace);


        for (var workplace: path) {
            workplace.unlockQueue();
            var waitingStatus = workplace.getWaitingStatus();
            waitingStatus.setCycle();
            waitingStatus.getNext().enter(waitingStatus);
        }

        return true;
    }

    @Override
    public Workplace switchTo(WorkplaceId wid) {
        try {
            mutex.acquire();

            var myStatus = statuses.get(Thread.currentThread().getId());
            var wantedWorkplace = workplaces.get(wid);
            var currentWorkplace = myStatus.getCurrent();
            myStatus.startWaiting(wantedWorkplace, getNewTicket());

            if (wantedWorkplace == myStatus.getCurrent()) {
                wantedWorkplace.enter(myStatus);

                mutex.release();
                return wantedWorkplace;
            }
            if (wantedWorkplace.isEmpty()) {
                wantedWorkplace.enter(myStatus);

                mutex.release();
                return wantedWorkplace;
            }
            if (!findAndFixCycle(myStatus)) {
                currentWorkplace.waitInQueue();
            } else {
                wantedWorkplace.enter(myStatus);
                myStatus.setCycle();
            }

            mutex.release();
            return wantedWorkplace;


        } catch (InterruptedException e) {
            throw new Panic();
        }

    }

    @Override
    public void leave() {
        try {
            mutex.acquire();
            var myStatus = statuses.get(Thread.currentThread().getId());
            var currentWorkplace = myStatus.getCurrent();
            currentWorkplace.leave();
            currentWorkplace.unlockUse();
            statuses.remove(Thread.currentThread().getId());
            unlockWorkplace(currentWorkplace, true);
            mutex.release();
        } catch (InterruptedException e) {
            throw new Panic();
        }
    }
}

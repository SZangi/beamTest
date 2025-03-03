
#include "ActionInitialization.hh"
#include "EventAction.hh"
#include "runAction.hh"
#include "PGA.hh"

void ActionInitialization::BuildForMaster() const
{
    auto rnAction = new runAction;
    SetUserAction(rnAction);
}

void ActionInitialization::Build() const
{
    SetUserAction(new PGA);

    auto rnAction = new runAction;
    SetUserAction(rnAction);

    auto evtAction = new EventAction(rnAction);
    SetUserAction(evtAction);
}
from fastapi import APIRouter, Depends, HTTPException
from models.registration import Registration
from models.user import User
from models.event import Event
from routes.auth import get_current_user, require_admin

router = APIRouter()

@router.get("/")
async def get_registrations(user: User = Depends(get_current_user)):
    registrations = await Registration.find(Registration.user_id == user.id).to_list()
    output = []
    for reg in registrations:
        event = await Event.get(reg.event_id)
        output.append({
            "id": str(reg.id),
            "user_id": str(reg.user_id),
            "event_id": str(reg.event_id),
            "event_title": event.title if event else "Unknown Event",
            "status": reg.status
        })
    return output

@router.post("/")
async def create_registration(event_name: str, user: User = Depends(get_current_user)):
    event = await Event.find_one(Event.title == event_name)
    registration = Registration(user_id = user.id, event_id = event.id, status = "Active")
    await registration.insert()
    return {"msg": "Registration created", "id": str(registration.id)}

@router.delete("/")
async def delete_registration(event_name: str, user: User = Depends(get_current_user)):
    event = await Event.find_one(Event.title == event_name)
    if not event:
        raise HTTPException(status_code=404, detail="Event not found")

    registration = await Registration.find_one({
        "$and": [
            {"event_id": event.id},
            {"user_id": user.id}
        ]
    })

    if not registration:
        raise HTTPException(status_code=404, detail="Your registration not found")

    registration.status = "Cancelled"
    await registration.save()
    return {"msg": "Registration cancelled", "id": str(registration.id)}

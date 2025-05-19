from fastapi import APIRouter, Depends, HTTPException
from models.registration import Registration
from models.user import User
from models.event import Event
from routes.auth import get_current_user, require_admin

router = APIRouter()

@router.get("/")
async def get_registrations(admin_user: User = Depends(require_admin)):
    return await Registration.find_all().to_list()

@router.post("/")
async def create_registration(event_name: str, user: User = Depends(get_current_user)):
    event = await Event.find_one(Event.title == event_name)
    registration = Registration(user_id = user.id, event_id = event.id, status = "Active")
    await registration.insert()
    return {"msg": "Registration created", "id": str(registration.id)}

@router.delete("/")
async def delete_registration(event_name: str, user: User = Depends(get_current_user)):
    event = await Event.find_one(Event.title == event_name)
    registration = await Registration.find_one(Registration.event_id == event.id)
    if not registration:
        raise HTTPException(status_code=404, detail="Registration not found")
    registration.status = "Cancelled"
    await registration.save()
    return {"msg": "Registration cancelled", "id": str(registration.id)}

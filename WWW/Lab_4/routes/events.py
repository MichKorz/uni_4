from fastapi import APIRouter
from models.event import Event

router = APIRouter()

@router.get("/")
async def get_events():
    return await Event.find_all().to_list()

@router.post("/")
async def create_event(event: Event):
    await event.insert()
    return {"msg": "Event created", "id": str(event.id)}

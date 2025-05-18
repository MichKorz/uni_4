from fastapi import APIRouter, Depends
from models.event import Event
from typing import Optional, List
from models.user import User
from routes.auth import require_admin

router = APIRouter()

@router.get("/")
async def get_events(
    skip: int = 0,
    limit: int = 10,
    sort_by: Optional[str] = "date",
    sort_dir: Optional[str] = "asc",  # or "desc"
    name: Optional[str] = None,
    category: Optional[str] = None
):
    query = {}

    if name:
        query["name"] = name
    if category:
        query["category"] = category

    sort_direction = 1 if sort_dir == "asc" else -1

    cursor = (
        Event.find(query)
        .sort([(sort_by, sort_direction)])
        .skip(skip)
        .limit(limit)
    )

    results = await cursor.to_list()
    return results

@router.post("/")
async def create_event(event: Event, admin_user: User = Depends(require_admin)):
    await event.insert()
    return {"msg": "Event created", "id": str(event.id)}

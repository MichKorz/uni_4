from beanie import Document
from datetime import datetime
from typing import List
from pydantic import BaseModel

class Event(Document):
    title: str
    description: str
    date: datetime
    location: str
    creator_id: str

    class Settings:
        name = "events"

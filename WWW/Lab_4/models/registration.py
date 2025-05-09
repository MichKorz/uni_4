from beanie import Document

class Registration(Document):
    user_id: str
    event_id: str
    status: str  # e.g. "registered", "cancelled"

    class Settings:
        name = "registrations"

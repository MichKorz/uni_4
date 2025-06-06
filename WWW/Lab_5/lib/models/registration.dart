class Registration {
  final String id;
  final String userId;
  final String eventTitle;
  final String status;

  Registration({
    required this.id,
    required this.userId,
    required this.eventTitle,
    required this.status,
  });

  factory Registration.fromJson(Map<String, dynamic> json) {
    return Registration(
      id: json['id'],
      userId: json['user_id'],
      eventTitle: json['event_title'], 
      status: json['status'],
    );
  }
}

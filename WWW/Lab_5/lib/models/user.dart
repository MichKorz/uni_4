class User {
  final String id;
  final String username;
  final String email;
  final String role;
  final String? token;

  User({
    required this.id,
    required this.username,
    required this.email,
    required this.role,
    this.token,
  });

  factory User.fromJson(Map<String, dynamic> json) {
    return User(
      id: json['_id'] ?? json['id'],
      username: json['username'],
      email: json['email'],
      role: json['role'],
      token: json.containsKey('access_token') ? json['access_token'] : null,
    );
  }
}

import 'dart:convert';
import 'package:http/http.dart' as http;
import 'package:shared_preferences/shared_preferences.dart';
import '../models/user.dart';
import '../services/api_service.dart';

class AuthService {
  static const String _registerUrl = '/auth/register';
  static const String _loginUrl = '/auth/login';

  Future<bool> register(String email, String username, String password, String role) async {
    final uri = Uri.parse('${ApiService.baseUrl}$_registerUrl');
    final response = await http.post(
      uri,
      headers: {'Content-Type': 'application/json'},
      body: json.encode({
        'email': email,
        'username': username,
        'password': password,
        'role': role,
      }),
    );
    return response.statusCode == 200;
  }

  Future<User?> login(String email, String password) async {
    final uri = Uri.parse('${ApiService.baseUrl}$_loginUrl');
    final response = await http.post(
      uri,
      headers: {'Content-Type': 'application/json'},
      body: json.encode({'email': email, 'password': password}),
    );
    if (response.statusCode == 200) {
      final data = json.decode(response.body);
      final token = data['access_token'];
      final prefs = await SharedPreferences.getInstance();
      await prefs.setString('token', token);
      // fetch user details after login
      final userUri = Uri.parse('${ApiService.baseUrl}/users/'); // admin only; skip
      // Instead, decode token to extract user id & role if needed client-side.
      return User(
        id: '',
        username: '',
        email: email,
        role: '',
        token: token,
      );
    }
    return null;
  }

  Future<void> logout() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.remove('token');
  }
}

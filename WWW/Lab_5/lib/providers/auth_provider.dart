import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';
import 'package:google_sign_in/google_sign_in.dart';
import '../utils/session.dart';

class AuthProvider extends ChangeNotifier {
  String? _token;
  bool _isAuth = false;

  bool get isAuthenticated => _isAuth;
  String? get token => _token;

  // Email/Password login
  Future<bool> login(String email, String password) async {
    final url = Uri.parse('http://127.0.0.1:8000/auth/login');
    try {
      final response = await http.post(
        url,
        headers: {'Content-Type': 'application/json'},
        body: json.encode({'email': email, 'password': password}),
      );

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        _token = data['access_token'];
        Session.token = _token;
        _isAuth = true;
        notifyListeners();
        return true;
      }
    } catch (_) {}
    return false;
  }

  // Register user
  Future<bool> register(String email, String username, String password, String role) async {
    final url = Uri.parse('http://127.0.0.1:8000/auth/register');
    try {
      final response = await http.post(
        url,
        headers: {'Content-Type': 'application/json'},
        body: json.encode({
          'email': email,
          'username': username,
          'password': password,
          'role': role,
        }),
      );
      return response.statusCode == 200;
    } catch (_) {
      return false;
    }
  }

  // Google login
  final GoogleSignIn _googleSignIn = GoogleSignIn();

  Future<bool> googleLogin() async {
    try {
      final account = await _googleSignIn.signIn();
      final authData = await account?.authentication;
      final idToken = authData?.idToken;

      print(idToken);

      if (idToken == null) return false;

      final response = await http.post(
        Uri.parse('http://127.0.0.1:8000/auth/google'),
        headers: {'Content-Type': 'application/json'},
        body: json.encode({'id_token_str': idToken}),
      );

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        _token = data['access_token'];
        Session.token = _token;
        _isAuth = true;
        notifyListeners();
        return true;
      }
    } catch (e) {
      print('[Google Login Error] $e');
    }
    return false;
  }

  // Logout
  Future<void> logout() async {
    _token = null;
    _isAuth = false;
    Session.token = null;
    notifyListeners();
  }
}

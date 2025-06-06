import 'package:flutter/material.dart';
// import 'package:shared_preferences/shared_preferences.dart';
import '../services/auth_service.dart';
import '../utils/session.dart';

class AuthProvider extends ChangeNotifier {
  final AuthService _authService = AuthService();
  bool _isAuth = false;

  bool get isAuthenticated => _isAuth;

  // Future<void> tryAutoLogin() async {
  //   final prefs = await SharedPreferences.getInstance();
  //   if (!prefs.containsKey('token')) {
  //     _isAuth = false;
  //     notifyListeners();
  //     return;
  //   }
  //   _isAuth = true;
  //   notifyListeners();
  // }

Future<bool> login(String email, String password) async {
  final user = await _authService.login(email, password);
  if (user != null && user.token != null) {
    Session.token = user.token;
    _isAuth = true;
    notifyListeners();
    return true;
  }
  return false;
}

  Future<bool> register(String email, String username, String password, String role) async {
    return await _authService.register(email, username, password, role);
  }

  Future<void> logout() async {
    await _authService.logout();
    _isAuth = false;
    Session.token = null;
    notifyListeners();
  }
}

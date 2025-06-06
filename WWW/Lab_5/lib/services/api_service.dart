import 'dart:convert';
import 'package:http/http.dart' as http;
import '../utils/session.dart';

class ApiService {
  static const String baseUrl = 'http://127.0.0.1:8000'; // update as needed

  Future<http.Response> getRequest(String endpoint) async {
    final uri = Uri.parse('$baseUrl$endpoint');
    return http.get(
      uri,
      headers: _headers(),
    );
  }

  Future<http.Response> postRequest(String endpoint, Map<String, dynamic> body) async {
    final uri = Uri.parse('$baseUrl$endpoint');
    return http.post(
      uri,
      headers: _headers(),
      body: json.encode(body),
    );
  }

  Future<http.Response> patchRequest(String endpoint, Map<String, dynamic> body) async {
    final uri = Uri.parse('$baseUrl$endpoint');
    return http.patch(
      uri,
      headers: _headers(),
      body: json.encode(body),
    );
  }

  Future<http.Response> deleteRequest(String endpoint, {Map<String, String>? params}) async {
    Uri uri = params != null
        ? Uri.parse('$baseUrl$endpoint').replace(queryParameters: params)
        : Uri.parse('$baseUrl$endpoint');

    return http.delete(
      uri,
      headers: _headers(),
    );
  }

  Map<String, String> _headers() {
    final token = Session.token;
    return {
      'Content-Type': 'application/json',
      if (token != null) 'Authorization': 'Bearer $token',
    };
  }

  Future<http.Response> postWithParams(String endpoint, Map<String, String> queryParams) async {
  final uri = Uri.parse('$baseUrl$endpoint').replace(queryParameters: queryParams);
  return http.post(
    uri,
    headers: _headers(),
  );
}
}

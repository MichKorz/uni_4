import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import '../providers/user_provider.dart';
import '../widgets/app_drawer.dart';

class ProfileScreen extends StatefulWidget {
  const ProfileScreen({super.key});
  @override
  State<ProfileScreen> createState() => _ProfileScreenState();
}

class _ProfileScreenState extends State<ProfileScreen> {
  bool _isLoading = false;
  final _formKey = GlobalKey<FormState>();
  String _newEmail = '';

  Future<void> _loadUsersIfAdmin() async {
    final userProv = Provider.of<UserProvider>(context, listen: false);
    await userProv.fetchAllUsers();
  }

  Future<void> _updateEmail() async {
    if (!_formKey.currentState!.validate()) return;
    _formKey.currentState!.save();

    setState(() => _isLoading = true);
    final success = await Provider.of<UserProvider>(context, listen: false)
        .updateEmail(_newEmail);
    setState(() => _isLoading = false);

    if (success) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Email updated')),
      );
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Update failed')),
      );
    }
  }

  @override
  void initState() {
    super.initState();
    _loadUsersIfAdmin();
  }

  @override
  Widget build(BuildContext context) {
    final userProv = Provider.of<UserProvider>(context);
    final users = userProv.users;
    return Scaffold(
      appBar: AppBar(title: const Text('Profile')),
      drawer: const AppDrawer(),
      body: Padding(
        padding: const EdgeInsets.all(16),
        child: ListView(
          children: [
            const Text('Update Email', style: TextStyle(fontSize: 18)),
            Form(
              key: _formKey,
              child: Row(
                children: [
                  Expanded(
                    child: TextFormField(
                      decoration: const InputDecoration(labelText: 'New Email'),
                      onSaved: (v) => _newEmail = v!.trim(),
                      validator: (v) =>
                          (v == null || !v.contains('@')) ? 'Valid email' : null,
                    ),
                  ),
                  const SizedBox(width: 12),
                  if (_isLoading)
                    const CircularProgressIndicator()
                  else
                    ElevatedButton(
                      onPressed: _updateEmail,
                      child: const Text('Save'),
                    ),
                ],
              ),
            ),
            const SizedBox(height: 20),
            const Divider(),
            const SizedBox(height: 10),
            const Text('All Users (Admin Only)', style: TextStyle(fontSize: 18)),
            ...users.map((u) => ListTile(
                  title: Text(u.username),
                  subtitle: Text('${u.email} (${u.role})'),
                )),
          ],
        ),
      ),
    );
  }
}

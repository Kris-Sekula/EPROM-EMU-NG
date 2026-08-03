import ast
import subprocess
import sys
import unittest
from pathlib import Path
from unittest import mock


SOFTWARE_DIR = Path(__file__).resolve().parents[1]
SCRIPT = SOFTWARE_DIR / 'EPROM_EMU_NG_2.1rc1.py'


def load_function_namespace():
    """Load imports/functions without executing the script's main program."""
    tree = ast.parse(SCRIPT.read_text(encoding='utf-8'), filename=str(SCRIPT))
    safe_nodes = [
        node for node in tree.body
        if isinstance(node, (ast.Import, ast.ImportFrom, ast.FunctionDef))
    ]
    namespace = {'__file__': str(SCRIPT), '__name__': 'eprom_test_module'}
    exec(compile(ast.Module(body=safe_nodes, type_ignores=[]), str(SCRIPT), 'exec'), namespace)
    return namespace


class TerminalInterfaceTests(unittest.TestCase):
    def setUp(self):
        self.namespace = load_function_namespace()

    def test_terminal_submit_returns_all_uploader_values(self):
        from prompt_toolkit.widgets import Button as RealButton
        from prompt_toolkit.widgets import TextArea as RealTextArea

        buttons = []
        fields = []
        created_apps = []

        class CapturingButton(RealButton):
            def __init__(self, text, handler=None, *args, **kwargs):
                super().__init__(text, handler=handler, *args, **kwargs)
                buttons.append(self)

        class CapturingTextArea(RealTextArea):
            def __init__(self, *args, **kwargs):
                super().__init__(*args, **kwargs)
                fields.append(self)

        class FakeApplication:
            def __init__(self, *args, **kwargs):
                self.layout = kwargs['layout']
                self.key_bindings = kwargs['key_bindings']
                self.style = kwargs['style']
                self.full_screen = kwargs['full_screen']
                created_apps.append(self)

            def run(self):
                # TextArea creation order: file, port, EPROM, start address.
                fields[0].text = r'E:\images\test.bin'
                fields[1].text = 'COM4'
                fields[2].text = '27256'
                fields[3].text = '0x1000'
                submit = next(button for button in buttons if button.text == 'Submit')
                submit.handler()

            def exit(self, *args, **kwargs):
                pass

            def invalidate(self):
                pass

        self.namespace['clear_screen'] = lambda: None
        self.namespace['get_com_ports'] = lambda: ['COM1', 'COM4']

        with mock.patch('prompt_toolkit.application.Application', FakeApplication), \
             mock.patch('prompt_toolkit.widgets.Button', CapturingButton), \
             mock.patch('prompt_toolkit.widgets.TextArea', CapturingTextArea):
            values = self.namespace['terminal_interface']()

        self.assertEqual(
            values,
            ('27256', 'COM4', 'n', 'n', '0x1000', 'y', r'E:\images\test.bin'))
        self.assertTrue(created_apps[0].full_screen)
        self.assertIsNotNone(created_apps[0].style)

    def test_main_terminal_bindings_include_safe_quit_and_navigation(self):
        source = SCRIPT.read_text(encoding='utf-8')
        self.assertIn("@kb.add('tab')", source)
        self.assertIn("@kb.add('s-tab')", source)
        self.assertIn("@kb.add('c-q')", source)
        self.assertIn("@kb.add('c-c')", source)
        self.assertIn('Ctrl-C is disabled here', source)

    def test_file_browser_has_cross_platform_navigation_and_hotkeys(self):
        source = SCRIPT.read_text(encoding='utf-8')
        self.assertIn("if os.name == 'nt':", source)
        self.assertIn("string.ascii_uppercase", source)
        self.assertIn("'[DIR]   ' + entry.name", source)
        self.assertIn("@browser_keys.add('c-o')", source)
        self.assertIn("@browser_keys.add('escape', 'o')", source)
        self.assertIn("@browser_keys.add('escape', 'c')", source)
        self.assertIn("@browser_list.control.key_bindings.add('enter')", source)

    def test_gui_window_loop_is_guarded_by_gui_mode(self):
        tree = ast.parse(SCRIPT.read_text(encoding='utf-8'))
        guarded = False
        for node in ast.walk(tree):
            if isinstance(node, ast.If) and isinstance(node.test, ast.Name) and node.test.id == 'gui':
                for child in ast.walk(node):
                    if (isinstance(child, ast.Call)
                            and isinstance(child.func, ast.Attribute)
                            and child.func.attr == 'read'
                            and isinstance(child.func.value, ast.Name)
                            and child.func.value.id == 'window'):
                        guarded = True
        self.assertTrue(guarded, 'window.read() must only execute when gui is true')


class StartupSmokeTests(unittest.TestCase):
    def run_script(self, *args):
        return subprocess.run(
            [sys.executable, str(SCRIPT), *args],
            cwd=SOFTWARE_DIR,
            text=True,
            capture_output=True,
            timeout=10)

    def test_help_starts_without_opening_an_interface(self):
        result = self.run_script('--help')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('--terminal', result.stdout)
        self.assertIn('--gui', result.stdout)

    def test_direct_cli_does_not_enter_gui_window_loop(self):
        result = self.run_script('__missing_test_image__.bin', 'COM_TEST_DOES_NOT_EXIST')
        combined = result.stdout + result.stderr
        self.assertNotIn("name 'window' is not defined", combined)
        self.assertIn('Failed to open port', combined)


if __name__ == '__main__':
    unittest.main()

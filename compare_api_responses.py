#!/usr/bin/env python3
"""
JSON API Response Comparison Tool

This script compares JSON response files between local and remote API implementations
to ensure format consistency across different versions.
"""

import json
import os
import sys
from pathlib import Path
from typing import Dict, List, Tuple, Any, Set
from dataclasses import dataclass
from datetime import datetime


@dataclass
class ComparisonResult:
    """Holds the result of comparing two JSON responses."""
    file_name: str
    identical: bool
    structure_match: bool
    differences: List[str]
    local_only_keys: Set[str]
    remote_only_keys: Set[str]
    type_mismatches: List[str]


class JSONComparator:
    """Compares JSON structures and formats between local and remote responses."""
    
    def __init__(self, local_dir: str, remote_dir: str):
        self.local_dir = Path(local_dir)
        self.remote_dir = Path(remote_dir)
        self.results: List[ComparisonResult] = []
    
    def get_json_structure(self, data: Any, path: str = "") -> Dict[str, str]:
        """Recursively extract the structure of a JSON object with data types."""
        structure = {}
        
        if isinstance(data, dict):
            for key, value in data.items():
                current_path = f"{path}.{key}" if path else key
                
                # Skip trackingId and msg fields as they can be ignored in comparison
                if key in ["trackingId", "msg"] and path == "":
                    continue
                    
                structure[current_path] = type(value).__name__
                if isinstance(value, (dict, list)):
                    structure.update(self.get_json_structure(value, current_path))
        elif isinstance(data, list):
            if data:  # If list is not empty
                # Check the type of the first element
                first_element = data[0]
                list_path = f"{path}[0]"
                structure[list_path] = type(first_element).__name__
                if isinstance(first_element, (dict, list)):
                    structure.update(self.get_json_structure(first_element, list_path))
        
        return structure
    
    def compare_json_files(self, local_file: Path, remote_file: Path) -> ComparisonResult:
        """Compare two JSON files and return detailed comparison results."""
        file_name = local_file.name
        
        try:
            # Read and parse JSON files
            with open(local_file, 'r', encoding='utf-8') as f:
                local_data = json.load(f)
            
            with open(remote_file, 'r', encoding='utf-8') as f:
                remote_data = json.load(f)
            
            # Get structures
            local_structure = self.get_json_structure(local_data)
            remote_structure = self.get_json_structure(remote_data)
            
            # Compare structures
            local_keys = set(local_structure.keys())
            remote_keys = set(remote_structure.keys())
            
            local_only = local_keys - remote_keys
            remote_only = remote_keys - local_keys
            common_keys = local_keys & remote_keys
            
            # Check for type mismatches in common keys
            type_mismatches = []
            for key in common_keys:
                if local_structure[key] != remote_structure[key]:
                    type_mismatches.append(f"{key}: local={local_structure[key]}, remote={remote_structure[key]}")
            
            # Generate differences list
            differences = []
            if local_only:
                differences.append(f"Keys only in local: {sorted(local_only)}")
            if remote_only:
                differences.append(f"Keys only in remote: {sorted(remote_only)}")
            if type_mismatches:
                differences.extend([f"Type mismatch - {tm}" for tm in type_mismatches])
            
            # Check if files are identical
            identical = local_data == remote_data
            structure_match = len(local_only) == 0 and len(remote_only) == 0 and len(type_mismatches) == 0
            
            return ComparisonResult(
                file_name=file_name,
                identical=identical,
                structure_match=structure_match,
                differences=differences,
                local_only_keys=local_only,
                remote_only_keys=remote_only,
                type_mismatches=type_mismatches
            )
            
        except json.JSONDecodeError as e:
            return ComparisonResult(
                file_name=file_name,
                identical=False,
                structure_match=False,
                differences=[f"JSON parsing error: {str(e)}"],
                local_only_keys=set(),
                remote_only_keys=set(),
                type_mismatches=[]
            )
        except Exception as e:
            return ComparisonResult(
                file_name=file_name,
                identical=False,
                structure_match=False,
                differences=[f"Error comparing files: {str(e)}"],
                local_only_keys=set(),
                remote_only_keys=set(),
                type_mismatches=[]
            )
    
    def find_response_files(self) -> Tuple[List[Path], List[Path]]:
        """Find all *_rsp.json files in local and remote directories."""
        local_files = list(self.local_dir.glob("*_rsp.json"))
        remote_files = list(self.remote_dir.glob("*_rsp.json"))
        
        local_files.sort()
        remote_files.sort()
        
        return local_files, remote_files
    
    def run_comparison(self) -> None:
        """Run the complete comparison process."""
        print(f"🔍 Comparing JSON responses between:")
        print(f"   Local:  {self.local_dir}")
        print(f"   Remote: {self.remote_dir}")
        print()
        
        local_files, remote_files = self.find_response_files()
        
        # Create filename to path mappings
        local_map = {f.name: f for f in local_files}
        remote_map = {f.name: f for f in remote_files}
        
        all_filenames = set(local_map.keys()) | set(remote_map.keys())
        
        print(f"Found {len(all_filenames)} unique response files to compare")
        print("-" * 80)
        
        for filename in sorted(all_filenames):
            local_file = local_map.get(filename)
            remote_file = remote_map.get(filename)
            
            if not local_file:
                print(f"❌ {filename}: Missing in local directory")
                continue
            
            if not remote_file:
                print(f"❌ {filename}: Missing in remote directory")
                continue
            
            result = self.compare_json_files(local_file, remote_file)
            self.results.append(result)
            
            # Print immediate result
            if result.identical:
                print(f"✅ {filename}: Identical")
            elif result.structure_match:
                print(f"🟡 {filename}: Same structure, different values")
            else:
                print(f"❌ {filename}: Different structure")
                for diff in result.differences[:3]:  # Show first 3 differences
                    print(f"   • {diff}")
                if len(result.differences) > 3:
                    print(f"   • ... and {len(result.differences) - 3} more differences")
    
    def generate_detailed_report(self, output_file: str | None = None) -> str:
        """Generate a detailed comparison report."""
        report_lines = []
        report_lines.append("=" * 80)
        report_lines.append("JSON API RESPONSE COMPARISON REPORT")
        report_lines.append("=" * 80)
        report_lines.append(f"Generated on: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append(f"Local directory:  {self.local_dir}")
        report_lines.append(f"Remote directory: {self.remote_dir}")
        report_lines.append("")
        
        # Summary statistics
        total_files = len(self.results)
        identical_files = sum(1 for r in self.results if r.identical)
        structure_match_files = sum(1 for r in self.results if r.structure_match)
        different_files = total_files - structure_match_files
        
        report_lines.append("SUMMARY")
        report_lines.append("-" * 40)
        report_lines.append(f"Total files compared: {total_files}")
        report_lines.append(f"Identical files: {identical_files}")
        report_lines.append(f"Same structure, different values: {structure_match_files - identical_files}")
        report_lines.append(f"Different structure: {different_files}")
        report_lines.append("")
        
        # Detailed results
        report_lines.append("DETAILED RESULTS")
        report_lines.append("-" * 40)
        
        for result in self.results:
            report_lines.append(f"\nFile: {result.file_name}")
            
            if result.identical:
                report_lines.append("  Status: ✅ IDENTICAL")
            elif result.structure_match:
                report_lines.append("  Status: 🟡 SAME STRUCTURE, DIFFERENT VALUES")
            else:
                report_lines.append("  Status: ❌ DIFFERENT STRUCTURE")
            
            if result.differences:
                report_lines.append("  Differences:")
                for diff in result.differences:
                    report_lines.append(f"    • {diff}")
        
        report_content = "\n".join(report_lines)
        
        if output_file:
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(report_content)
            print(f"\n📄 Detailed report saved to: {output_file}")
        
        return report_content


def main():
    """Main function to run the comparison tool."""
    script_dir = Path(__file__).parent
    local_dir = script_dir / "build" / "PJsonEditor" / "tests" / "local"
    remote_dir = script_dir / "build" / "PJsonEditor" / "tests" / "remote"

    # Check if directories exist
    if not local_dir.exists():
        print(f"❌ Local directory not found: {local_dir}")
        sys.exit(1)
    
    if not remote_dir.exists():
        print(f"❌ Remote directory not found: {remote_dir}")
        sys.exit(1)
    
    # Run comparison
    comparator = JSONComparator(str(local_dir), str(remote_dir))
    comparator.run_comparison()
    
    print("\n" + "=" * 80)
    
    # Generate detailed report
    report_file = script_dir / f"api_comparison_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.txt"
    comparator.generate_detailed_report(str(report_file))
    
    # Summary
    total = len(comparator.results)
    if total == 0:
        print("No response files found to compare.")
        return
    
    identical = sum(1 for r in comparator.results if r.identical)
    structure_match = sum(1 for r in comparator.results if r.structure_match)
    
    print(f"\n📊 FINAL SUMMARY:")
    print(f"   • {identical}/{total} files are identical")
    print(f"   • {structure_match}/{total} files have matching structure")
    print(f"   • {total - structure_match}/{total} files have structural differences")
    
    if structure_match == total:
        print("\n🎉 All response formats match! Your API implementations are consistent.")
    else:
        print(f"\n⚠️  {total - structure_match} files have structural differences that need attention.")


if __name__ == "__main__":
    main()
----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 09/21/2025 10:47:37 PM
-- Design Name: 
-- Module Name: and5_struct - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity and5_struct is
    Port ( A : in STD_LOGIC_VECTOR(4 downto 0);
           Q : out STD_LOGIC);
end and5_struct;

architecture Structural of and5_struct is

component and2 is
    Port (
        A : in  STD_LOGIC;
        B : in  STD_LOGIC;
        Q : out STD_LOGIC
    );
end component;

signal chain : STD_LOGIC_VECTOR(3 downto 0);

begin
    G1: and2 port map (A => A(0), B => A(1), Q => chain(0));
    GEN_AND: for i in 1 to 3 generate
        U: and2 port map (
            A => chain(i-1),
            B => A(i+1),
            Q => chain(i)
        );
    end generate;
    Q <= chain(3);
end Structural;
